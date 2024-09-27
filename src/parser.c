/*
 * @parser.c
 *
 * @brief C for Stack VM
 * @details
 * This is based on other projects:
 *   A minimalist C compiler with x86_64 code generation: https://github.com/jserv/MazuCC
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 * @date 2024
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stackvm_c_compiler
 */

#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_stackvm.h"
#include "parser.h"
#include "dict.h"
#include "list.h"
#include "verbose.h"
#include "lexer.h"

extern void **mkstr;
extern long mkstr_qty;

list_t *ctypes = &EMPTY_LIST;
list_t *strings = &EMPTY_LIST;
list_t *flonums = &EMPTY_LIST;

static dict_t *globalenv = &EMPTY_DICT;
static dict_t *localenv = NULL;
static dict_t *struct_defs = &EMPTY_DICT;
static dict_t *union_defs = &EMPTY_DICT;
static list_t *localvars = NULL;

static ctype_t *ctype_void = &(ctype_t )  { CTYPE_VOID, 0, NULL };
static ctype_t *ctype_int = &(ctype_t )   { CTYPE_INT, 4, NULL };
static ctype_t *ctype_long = &(ctype_t )  { CTYPE_LONG, 8, NULL };
static ctype_t *ctype_char = &(ctype_t )  { CTYPE_CHAR, 1, NULL };
static ctype_t *ctype_float = &(ctype_t ) { CTYPE_FLOAT, 4, NULL };
#ifdef ALLOW_DOUBLE
static ctype_t *ctype_double = &(ctype_t ) { CTYPE_DOUBLE, 8, NULL };
#endif
static int labelseq = 0;

ast_t* ast_uop(int type, ctype_t *ctype, ast_t *operand) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = type;
    r->ctype = ctype;
    r->operand = operand;
    return r;
}

ast_t* ast_binop(int type, ast_t *left, ast_t *right) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = type;
    r->ctype = result_type(type, left->ctype, right->ctype);
    if (type != '=' && convert_array(left->ctype)->type != CTYPE_PTR && convert_array(right->ctype)->type == CTYPE_PTR) {
        r->left = right;
        r->right = left;
    } else {
        r->left = left;
        r->right = right;
    }
    return r;
}

ast_t* ast_inttype(ctype_t *ctype, long val) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_LITERAL;
    r->ctype = ctype;
    r->ival = val;
    return r;
}

ast_t* ast_double(double val) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_LITERAL;
#ifdef ALLOW_DOUBLE
    r->ctype = ctype_double;
#else
    r->ctype = ctype_float;
#endif
    r->fval = val;
    list_push(flonums, r);
    return r;
}

char* make_label(void) {
    string_t s = make_string();
    add_str_ptr(mkstr, mkstr_qty, s.body);
    string_appendf(&s, ".L%d", labelseq++);
    return get_cstring(s);
}

ast_t* ast_lvar(ctype_t *ctype, char *name) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_LVAR;
    r->ctype = ctype;
    r->varname = name;
    dict_put(localenv, name, r);
    if (localvars)
        list_push(localvars, r);
    return r;
}

ast_t* ast_gvar(ctype_t *ctype, char *name, bool filelocal) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_GVAR;
    r->ctype = ctype;
    r->varname = name;
    r->glabel = filelocal ? make_label() : name;
    dict_put(globalenv, name, r);
    return r;
}

ast_t* ast_string(char *str) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_STRING;
    r->ctype = make_array_type(ctype_char, strlen(str) + 1);
    r->sval = str;
    r->slabel = make_label();
    return r;
}

ast_t* ast_funcall(ctype_t *ctype, char *fname, list_t *args) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_FUNCALL;
    r->ctype = ctype;
    r->fname = fname;
    r->args = args;
    return r;
}

ast_t* ast_func(ctype_t *rettype, char *fname, list_t *params, ast_t *body, list_t *localvars) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_FUNC;
    r->ctype = rettype;
    r->fname = fname;
    r->params = params;
    r->localvars = localvars;
    r->body = body;
    return r;
}

ast_t* ast_decl(ast_t *var, ast_t *init) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_DECL;
    r->ctype = NULL;
    r->declvar = var;
    r->declinit = init;
    return r;
}

ast_t* ast_array_init(list_t *arrayinit) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_ARRAY_INIT;
    r->ctype = NULL;
    r->arrayinit = arrayinit;
    return r;
}

ast_t* ast_if(ast_t *cond, ast_t *then, ast_t *els) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_IF;
    r->ctype = NULL;
    r->cond = cond;
    r->then = then;
    r->els = els;
    return r;
}

ast_t* ast_ternary(ctype_t *ctype, ast_t *cond, ast_t *then, ast_t *els) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_TERNARY;
    r->ctype = ctype;
    r->cond = cond;
    r->then = then;
    r->els = els;
    return r;
}

ast_t* ast_for(ast_t *init, ast_t *cond, ast_t *step, ast_t *body) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_FOR;
    r->ctype = NULL;
    r->forinit = init;
    r->forcond = cond;
    r->forstep = step;
    r->forbody = body;
    return r;
}

ast_t* ast_return(ast_t *retval) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_RETURN;
    r->ctype = NULL;
    r->retval = retval;
    return r;
}

ast_t* ast_compound_stmt(list_t *stmts) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_COMPOUND_STMT;
    r->ctype = NULL;
    r->stmts = stmts;
    return r;
}

ast_t* ast_struct_ref(ctype_t *ctype, ast_t *struc, char *name) {
    ast_t *r = malloc(sizeof(ast_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = AST_STRUCT_REF;
    r->ctype = ctype;
    r->struc = struc;
    r->field = name;
    return r;
}

ctype_t* make_ptr_type(ctype_t *ctype) {
    ctype_t *r = malloc(sizeof(ctype_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = CTYPE_PTR;
    r->ptr = ctype;
    r->size = 8;
    list_push(ctypes, r);
    return r;
}

ctype_t* make_array_type(ctype_t *ctype, int len) {
    ctype_t *r = malloc(sizeof(ctype_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = CTYPE_ARRAY;
    r->ptr = ctype;
    r->size = (len < 0) ? -1 : ctype->size * len;
    r->len = len;
    list_push(ctypes, r);
    return r;
}

ctype_t* make_struct_field_type(ctype_t *ctype, int offset) {
    ctype_t *r = malloc(sizeof(ctype_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    memcpy(r, ctype, sizeof(ctype_t));
    r->offset = offset;
    list_push(ctypes, r);
    return r;
}

ctype_t* make_struct_type(dict_t *fields, int size) {
    ctype_t *r = malloc(sizeof(ctype_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->type = CTYPE_STRUCT;
    r->fields = fields;
    r->size = size;
    list_push(ctypes, r);
    return r;
}

bool is_inttype(ctype_t *ctype) {
    return ctype->type == CTYPE_CHAR || ctype->type == CTYPE_INT || ctype->type == CTYPE_LONG;
}

bool is_flotype(ctype_t *ctype) {
#ifdef ALLOW_DOUBLE
    return ctype->type == CTYPE_FLOAT || ctype->type == CTYPE_DOUBLE;
#else
    return ctype->type == CTYPE_FLOAT;
#endif
}

void ensure_lvalue(ast_t *ast) {
    switch (ast->type) {
        case AST_LVAR:
        case AST_GVAR:
        case AST_DEREF:
        case AST_STRUCT_REF:
            return;
        default:
            error("lvalue expected, but got %s", ast_to_string(ast, true));
    }
}

void expect(char punct) {
    token_t tok = read_token();
    if (!is_punct(tok, punct))
        error("'%c' expected, but got %s", punct, token_to_string(tok));
}

bool is_ident(const token_t tok, char *s) {
    return get_ttype(tok) == TTYPE_IDENT && !strcmp(get_ident(tok), s);
}

bool is_right_assoc(const token_t tok) {
    return get_punct(tok) == '=';
}

int eval_intexpr(ast_t *ast) {
    switch (ast->type) {
        case AST_LITERAL:
            if (is_inttype(ast->ctype))
                return ast->ival;
            error("Integer expression expected, but got %s", ast_to_string(ast, true));
        case '+':
            return eval_intexpr(ast->left) + eval_intexpr(ast->right);
        case '-':
            return eval_intexpr(ast->left) - eval_intexpr(ast->right);
        case '*':
            return eval_intexpr(ast->left) * eval_intexpr(ast->right);
        case '/':
            return eval_intexpr(ast->left) / eval_intexpr(ast->right);
        case PUNCT_LSHIFT:
            return eval_intexpr(ast->left) << eval_intexpr(ast->right);
        case PUNCT_RSHIFT:
            return eval_intexpr(ast->left) >> eval_intexpr(ast->right);
        default:
            error("Integer expression expected, but got %s", ast_to_string(ast, true));
            return 0; /* non-reachable */
    }
}

int priority(const token_t tok) {
    switch (get_punct(tok)) {
        case '[':
        case '.':
        case PUNCT_ARROW:
            return 1;
        case PUNCT_INC:
        case PUNCT_DEC:
            return 2;
        case '*':
        case '/':
            return 3;
        case '+':
        case '-':
            return 4;
        case PUNCT_LSHIFT:
        case PUNCT_RSHIFT:
            return 5;
        case '<':
        case '>':
            return 6;
        case '&':
            return 8;
        case '|':
            return 10;
        case PUNCT_EQ:
            return 7;
        case PUNCT_LOGAND:
            return 11;
        case PUNCT_LOGOR:
            return 12;
        case '?':
            return 13;
        case '=':
            return 14;
        default:
            return -1;
    }
}

ast_t* read_func_args(char *fname) {
    list_t *args = make_list();
    while (1) {
        token_t tok = read_token();
        if (is_punct(tok, ')'))
            break;
        unget_token(tok);
        list_push(args, read_expr());
        tok = read_token();
        if (is_punct(tok, ')'))
            break;
        if (!is_punct(tok, ','))
            error("Unexpected token: '%s'", token_to_string(tok));
    }
    if (MAX_ARGS < list_len(args))
        error("Too many arguments: %s", fname);
    return ast_funcall(ctype_int, fname, args);
}

ast_t* read_ident_or_func(char *name) {
    token_t tok = read_token();
    if (is_punct(tok, '('))
        return read_func_args(name);
    unget_token(tok);
    ast_t *v = dict_get(localenv, name);
    if (!v)
        error("Undefined varaible: %s", name);
    return v;
}

bool is_long_token(char *p) {
    for (; *p; p++) {
        if (!isdigit(*p))
            return (*p == 'L' || *p == 'l') && p[1] == '\0';
    }
    return false;
}

bool is_int_token(char *p) {
    for (; *p; p++)
        if (!isdigit(*p))
            return false;
    return true;
}

bool is_float_token(char *p) {
    for (; *p; p++)
        if (!isdigit(*p))
            break;
    if (*p++ != '.')
        return false;
    for (; *p; p++)
        if (!isdigit(*p))
            return false;
    return true;
}

ast_t* read_prim(void) {
    token_t tok = read_token();
    switch (get_ttype(tok)) {
        case TTYPE_NULL:
            return NULL;
        case TTYPE_IDENT:
            return read_ident_or_func(get_ident(tok));
        case TTYPE_NUMBER: {
            char *number = get_number(tok);
            if (is_long_token(number))
                return ast_inttype(ctype_long, atol(number));
            if (is_int_token(number)) {
                long val = atol(number);
                if (val & ~(long) UINT_MAX)
                    return ast_inttype(ctype_long, val);
                return ast_inttype(ctype_int, val);
            }
            if (is_float_token(number))
                return ast_double(atof(number));
            error("Malformed number: %s", token_to_string(tok));
        }
        case TTYPE_CHAR:
            return ast_inttype(ctype_char, get_char(tok));
        case TTYPE_STRING: {
            ast_t *r = ast_string(get_strtok(tok));
            list_push(strings, r);
            return r;
        }
        case TTYPE_PUNCT:
            unget_token(tok);
            return NULL;
        default:
            error("internal error: unknown token type: %d", get_ttype(tok));
            return NULL; /* non-reachable */
    }
}

ctype_t* result_type_int(jmp_buf *jmpbuf, char op, ctype_t *a, ctype_t *b) {
    if (a->type > b->type)
        swap(a, b);
    if (b->type == CTYPE_PTR) {
        if (op == '=')
            return a;
        if (op != '+' && op != '-')
            goto err;
        if (!is_inttype(a))
            goto err;
        return b;
    }
    switch (a->type) {
        case CTYPE_VOID:
            goto err;
        case CTYPE_CHAR:
        case CTYPE_INT:
            switch (b->type) {
                case CTYPE_CHAR:
                case CTYPE_INT:
                    return ctype_int;
                case CTYPE_LONG:
                    return ctype_long;
                case CTYPE_FLOAT:
#ifdef ALLOW_DOUBLE
                case CTYPE_DOUBLE:
                    return ctype_double;
#endif
                    return ctype_float;
                case CTYPE_ARRAY:
                case CTYPE_PTR:
                    return b;
            }
            error("internal error");
        case CTYPE_LONG:
            switch (b->type) {
                case CTYPE_LONG:
                    return ctype_long;
                case CTYPE_FLOAT:
#ifdef ALLOW_DOUBLE
                case CTYPE_DOUBLE:
                    return ctype_double;
#else
                    return ctype_float;
#endif
                case CTYPE_ARRAY:
                case CTYPE_PTR:
                    return b;
            }
            error("internal error");
        case CTYPE_FLOAT:
#ifdef ALLOW_DOUBLE
            if (b->type == CTYPE_FLOAT || b->type == CTYPE_DOUBLE)
                return ctype_double;
#else
            if (b->type == CTYPE_FLOAT)
                return ctype_float;
#endif
            goto err;
#ifdef ALLOW_DOUBLE
        case CTYPE_DOUBLE:
            if (b->type == CTYPE_DOUBLE)
                return ctype_double;
            goto err;
#endif
        case CTYPE_ARRAY:
            if (b->type != CTYPE_ARRAY)
                goto err;
            return result_type_int(jmpbuf, op, a->ptr, b->ptr);
        default:
            error("internal error: %s %s", ctype_to_string(a), ctype_to_string(b));
    }
err:
    longjmp(*jmpbuf, 1);
}

ast_t* read_subscript_expr(ast_t *ast) {
    ast_t *sub = read_expr();
    expect(']');
    ast_t *t = ast_binop('+', ast, sub);
    return ast_uop(AST_DEREF, t->ctype->ptr, t);
}

ctype_t* convert_array(ctype_t *ctype) {
    if (ctype->type != CTYPE_ARRAY)
        return ctype;
    return make_ptr_type(ctype->ptr);
}

ctype_t* result_type(char op, ctype_t *a, ctype_t *b) {
    jmp_buf jmpbuf;
    if (setjmp(jmpbuf) == 0)
        return result_type_int(&jmpbuf, op, convert_array(a), convert_array(b));
    error("incompatible operands: %c: <%s> and <%s>", op, ctype_to_string(a), ctype_to_string(b));
    return NULL; /* non-reachable */
}

ast_t* read_unary_expr(void) {
    token_t tok = read_token();
    if (get_ttype(tok) != TTYPE_PUNCT) {
        unget_token(tok);
        return read_prim();
    }
    if (is_punct(tok, '(')) {
        ast_t *r = read_expr();
        expect(')');
        return r;
    }
    if (is_punct(tok, '&')) {
        ast_t *operand = read_unary_expr();
        ensure_lvalue(operand);
        return ast_uop(AST_ADDR, make_ptr_type(operand->ctype), operand);
    }
    if (is_punct(tok, '!')) {
        ast_t *operand = read_unary_expr();
        return ast_uop('!', ctype_int, operand);
    }
    if (is_punct(tok, '*')) {
        ast_t *operand = read_unary_expr();
        ctype_t *ctype = convert_array(operand->ctype);
        if (ctype->type != CTYPE_PTR)
            error("pointer type expected, but got %s", ast_to_string(operand, true));
        if (ctype->ptr == ctype_void)
            error("pointer to void can not be dereferenced, but got %s", ast_to_string(operand, true));
        return ast_uop(AST_DEREF, operand->ctype->ptr, operand);
    }
    unget_token(tok);
    return read_prim();
}

ast_t* read_cond_expr(ast_t *cond) {
    ast_t *then = read_expr();
    expect(':');
    ast_t *els = read_expr();
    return ast_ternary(then->ctype, cond, then, els);
}

ast_t* read_struct_field(ast_t *struc) {
    if (struc->ctype->type != CTYPE_STRUCT)
        error("struct expected, but got %s", ast_to_string(struc, true));
    token_t name = read_token();
    if (get_ttype(name) != TTYPE_IDENT)
        error("field name expected, but got %s", token_to_string(name));
    char *ident = get_ident(name);
    ctype_t *field = dict_get(struc->ctype->fields, ident);
    return ast_struct_ref(field, struc, ident);
}

ast_t* read_expr_int(int prec) {
    ast_t *ast = read_unary_expr();
    if (!ast)
        return NULL;
    while (1) {
        token_t tok = read_token();
        if (get_ttype(tok) != TTYPE_PUNCT) {
            unget_token(tok);
            return ast;
        }
        int prec2 = priority(tok);
        if (prec2 < 0 || prec <= prec2) {
            unget_token(tok);
            return ast;
        }
        if (is_punct(tok, '?')) {
            ast = read_cond_expr(ast);
            continue;
        }
        if (is_punct(tok, '.')) {
            ast = read_struct_field(ast);
            continue;
        }
        if (is_punct(tok, PUNCT_ARROW)) {
            if (ast->ctype->type != CTYPE_PTR)
                error("pointer type expected, but got %s %s", ctype_to_string(ast->ctype), ast_to_string(ast, true));
            ast = ast_uop(AST_DEREF, ast->ctype->ptr, ast);
            ast = read_struct_field(ast);
            continue;
        }
        if (is_punct(tok, '[')) {
            ast = read_subscript_expr(ast);
            continue;
        }
        // this is BUG!! ++ should be in read_unary_expr() , I think.
        if (is_punct(tok, PUNCT_INC) || is_punct(tok, PUNCT_DEC)) {
            ensure_lvalue(ast);
            ast = ast_uop(get_punct(tok), ast->ctype, ast);
            continue;
        }
        if (is_punct(tok, '='))
            ensure_lvalue(ast);
        ast_t *rest = read_expr_int(prec2 + (is_right_assoc(tok) ? 1 : 0));
        if (!rest)
            error("second operand missing");
        if (is_punct(tok, PUNCT_LSHIFT) || is_punct(tok, PUNCT_RSHIFT)) {
            if ((ast->ctype != ctype_int && ast->ctype != ctype_char) || (rest->ctype != ctype_int && rest->ctype != ctype_char))
                error("invalid operand to shift");
        }
        ast = ast_binop(get_punct(tok), ast, rest);
    }
}

ast_t* read_expr(void) {
    return read_expr_int(MAX_OP_PRIO);
}

ctype_t* get_ctype(const token_t tok) {
    if (get_ttype(tok) != TTYPE_IDENT)
        return NULL;
    char *ident = get_ident(tok);
    if (!strcmp(ident, "void"))
        return ctype_void;
    if (!strcmp(ident, "int"))
        return ctype_int;
    if (!strcmp(ident, "long"))
        return ctype_long;
    if (!strcmp(ident, "char"))
        return ctype_char;
    if (!strcmp(ident, "float"))
        return ctype_float;
#ifdef ALLOW_DOUBLE
    if (!strcmp(ident, "double"))
        return ctype_double;
#endif
    return NULL;
}

bool is_type_keyword(const token_t tok) {
    return get_ctype(tok) || is_ident(tok, "struct") || is_ident(tok, "union");
}

ast_t* read_decl_array_init_int(ctype_t *ctype) {
    token_t tok = read_token();
    if (ctype->ptr->type == CTYPE_CHAR && get_ttype(tok) == TTYPE_STRING)
        return ast_string(get_strtok(tok));
    if (!is_punct(tok, '{'))
        error("Expected an initializer list for %s, but got %s", ctype_to_string(ctype), token_to_string(tok));
    list_t *initlist = make_list();
    while (1) {
        token_t tok = read_token();
        if (is_punct(tok, '}'))
            break;
        unget_token(tok);
        ast_t *init = read_expr();
        list_push(initlist, init);
        result_type('=', init->ctype, ctype->ptr);
        tok = read_token();
        if (!is_punct(tok, ','))
            unget_token(tok);
    }
    return ast_array_init(initlist);
}

char* read_struct_union_tag(void) {
    token_t tok = read_token();
    if (get_ttype(tok) == TTYPE_IDENT)
        return get_ident(tok);
    unget_token(tok);
    return NULL;
}

dict_t* read_struct_union_fields(void) {
    dict_t *r = make_dict(NULL);
    expect('{');
    while (1) {
        if (!is_type_keyword(peek_token()))
            break;
        token_t name;
        ctype_t *fieldtype = read_decl_int(&name);
        dict_put(r, get_ident(name), make_struct_field_type(fieldtype, 0));
        expect(';');
    }
    expect('}');
    return r;
}

ctype_t* read_union_def(void) {
    char *tag = read_struct_union_tag();
    ctype_t *ctype = dict_get(union_defs, tag);
    if (ctype)
        return ctype;
    dict_t *fields = read_struct_union_fields();
    int maxsize = 0;
    for (iter_t i = list_iter(dict_values(fields)); !iter_end(i);) {
        ctype_t *fieldtype = iter_next(&i);
        maxsize = (maxsize < fieldtype->size) ? fieldtype->size : maxsize;
    }
    ctype_t *r = make_struct_type(fields, maxsize);
    if (tag)
        dict_put(union_defs, tag, r);
    return r;
}

ctype_t* read_struct_def(void) {
    char *tag = read_struct_union_tag();
    ctype_t *ctype = dict_get(struct_defs, tag);
    if (ctype)
        return ctype;
    dict_t *fields = read_struct_union_fields();
    int offset = 0;
    for (iter_t i = list_iter(dict_values(fields)); !iter_end(i);) {
        ctype_t *fieldtype = iter_next(&i);
        int size = (fieldtype->size < MAX_ALIGN) ? fieldtype->size : MAX_ALIGN;
        if (offset % size != 0)
            offset += size - offset % size;
        fieldtype->offset = offset;
        offset += fieldtype->size;
    }
    ctype_t *r = make_struct_type(fields, offset);
    if (tag)
        dict_put(struct_defs, tag, r);
    return r;
}

ctype_t* read_decl_spec(void) {
    token_t tok = read_token();
    ctype_t *ctype = is_ident(tok, "struct") ? read_struct_def() : is_ident(tok, "union") ? read_union_def() : get_ctype(tok);
    if (!ctype)
        error("Type expected, but got %s", token_to_string(tok));
    while (1) {
        tok = read_token();
        if (!is_punct(tok, '*')) {
            unget_token(tok);
            return ctype;
        }
        ctype = make_ptr_type(ctype);
    }
}

ast_t* read_decl_init_val(ast_t *var) {
    if (var->ctype->type == CTYPE_ARRAY) {
        ast_t *init = read_decl_array_init_int(var->ctype);
        int len = (init->type == AST_STRING) ? strlen(init->sval) + 1 : list_len(init->arrayinit);
        if (var->ctype->len == -1) {
            var->ctype->len = len;
            var->ctype->size = len * var->ctype->ptr->size;
        } else if (var->ctype->len != len) {
            error("Invalid array initializer: expected %d items but got %d", var->ctype->len, len);
        }
        expect(';');
        return ast_decl(var, init);
    }
    ast_t *init = read_expr();
    expect(';');
    if (var->type == AST_GVAR)
        init = ast_inttype(ctype_int, eval_intexpr(init));
    return ast_decl(var, init);
}

ctype_t* read_array_dimensions_int(ctype_t *basetype) {
    token_t tok = read_token();
    if (!is_punct(tok, '[')) {
        unget_token(tok);
        return NULL;
    }
    int dim = -1;
    if (!is_punct(peek_token(), ']')) {
        ast_t *size = read_expr();
        dim = eval_intexpr(size);
    }
    expect(']');
    ctype_t *sub = read_array_dimensions_int(basetype);
    if (sub) {
        if (sub->len == -1 && dim == -1)
            error("Array size is not specified");
        return make_array_type(sub, dim);
    }
    return make_array_type(basetype, dim);
}

ctype_t* read_array_dimensions(ctype_t *basetype) {
    ctype_t *ctype = read_array_dimensions_int(basetype);
    return ctype ? ctype : basetype;
}

ast_t* read_decl_init(ast_t *var) {
    token_t tok = read_token();
    if (is_punct(tok, '='))
        return read_decl_init_val(var);
    if (var->ctype->len == -1)
        error("Missing array initializer");
    unget_token(tok);
    expect(';');
    return ast_decl(var, NULL);
}

ctype_t* read_decl_int(token_t *name) {
    ctype_t *ctype = read_decl_spec();
    *name = read_token();
    if (get_ttype((*name)) != TTYPE_IDENT)
        error("Identifier expected, but got %s", token_to_string(*name));
    return read_array_dimensions(ctype);
}

ast_t* read_decl(void) {
    token_t varname;
    ctype_t *ctype = read_decl_int(&varname);
    if (ctype == ctype_void)
        error("Storage size of '%s' is not known", token_to_string(varname));
    ast_t *var = ast_lvar(ctype, get_ident(varname));
    return read_decl_init(var);
}

ast_t* read_if_stmt(void) {
    expect('(');
    ast_t *cond = read_expr();
    expect(')');
    ast_t *then = read_stmt();
    token_t tok = read_token();
    if (get_ttype(tok) != TTYPE_IDENT || strcmp(get_ident(tok), "else")) {
        unget_token(tok);
        return ast_if(cond, then, NULL);
    }
    ast_t *els = read_stmt();
    return ast_if(cond, then, els);
}

ast_t* read_opt_decl_or_stmt(void) {
    token_t tok = read_token();
    if (is_punct(tok, ';'))
        return NULL;
    unget_token(tok);
    return read_decl_or_stmt();
}

ast_t* read_opt_expr(void) {
    token_t tok = read_token();
    if (is_punct(tok, ';'))
        return NULL;
    unget_token(tok);
    ast_t *r = read_expr();
    expect(';');
    return r;
}

ast_t* read_for_stmt(void) {
    expect('(');
    localenv = make_dict(localenv);
    ast_t *init = read_opt_decl_or_stmt();
    ast_t *cond = read_opt_expr();
    ast_t *step = is_punct(peek_token(), ')') ? NULL : read_expr();
    expect(')');
    ast_t *body = read_stmt();
    localenv = dict_parent(localenv);
    return ast_for(init, cond, step, body);
}

ast_t* read_return_stmt(void) {
    ast_t *retval = read_expr();
    expect(';');
    return ast_return(retval);
}

ast_t* read_stmt(void) {
    token_t tok = read_token();
    if (is_ident(tok, "if"))
        return read_if_stmt();
    if (is_ident(tok, "for"))
        return read_for_stmt();
    if (is_ident(tok, "return"))
        return read_return_stmt();
    if (is_punct(tok, '{'))
        return read_compound_stmt();
    unget_token(tok);
    ast_t *r = read_expr();
    expect(';');
    return r;
}

ast_t* read_decl_or_stmt(void) {
    token_t tok = peek_token();
    if (get_ttype(tok) == TTYPE_NULL)
        return NULL;
    return is_type_keyword(tok) ? read_decl() : read_stmt();
}

ast_t* read_compound_stmt(void) {
    localenv = make_dict(localenv);
    list_t *list = make_list();
    while (1) {
        ast_t *stmt = read_decl_or_stmt();
        if (stmt)
            list_push(list, stmt);
        if (!stmt)
            break;
        token_t tok = read_token();
        if (is_punct(tok, '}'))
            break;
        unget_token(tok);
    }
    localenv = dict_parent(localenv);
    return ast_compound_stmt(list);
}

list_t* read_params(void) {
    list_t *params = make_list();
    token_t tok = read_token();
    if (is_punct(tok, ')'))
        return params;
    unget_token(tok);
    while (1) {
        ctype_t *ctype = read_decl_spec();
        token_t pname = read_token();
        if (get_ttype(pname) != TTYPE_IDENT)
            error("Identifier expected, but got %s", token_to_string(pname));
        ctype = read_array_dimensions(ctype);
        if (ctype->type == CTYPE_ARRAY)
            ctype = make_ptr_type(ctype->ptr);
        list_push(params, ast_lvar(ctype, get_ident(pname)));
        token_t tok = read_token();
        if (is_punct(tok, ')'))
            return params;
        if (!is_punct(tok, ','))
            error("Comma expected, but got %s", token_to_string(tok));
    }
}

ast_t* read_func_def(ctype_t *rettype, char *fname) {
    expect('(');
    localenv = make_dict(globalenv);
    list_t *params = read_params();
    expect('{');
    localenv = make_dict(localenv);
    localvars = make_list();
    ast_t *body = read_compound_stmt();
    ast_t *r = ast_func(rettype, fname, params, body, localvars);
    localenv = dict_parent(localenv);
    localenv = dict_parent(localenv);
    localvars = NULL;
    return r;
}

ast_t* read_decl_or_func_def(void) {
    token_t tok = peek_token();
    if (get_ttype(tok) == TTYPE_NULL)
        return NULL;
    ctype_t *ctype = read_decl_spec();
    token_t name = read_token();
    char *ident;
    if (get_ttype(name) != TTYPE_IDENT)
        error("Identifier expected, but got %s", token_to_string(name));
    ident = get_ident(name);
    tok = peek_token();
    if (is_punct(tok, '('))
        return read_func_def(ctype, ident);
    if (ctype == ctype_void)
        error("Storage size of '%s' is not known", token_to_string(name));
    ctype = read_array_dimensions(ctype);
    if (is_punct(tok, '=') || ctype->type == CTYPE_ARRAY) {
        ast_t *var = ast_gvar(ctype, ident, false);
        return read_decl_init(var);
    }
    if (is_punct(tok, ';')) {
        read_token();
        ast_t *var = ast_gvar(ctype, ident, false);
        return ast_decl(var, NULL);
    }
    error("Don't know how to handle %s", token_to_string(tok));
    return NULL; /* non-reachable */
}

list_t* read_toplevels(void) {
    list_t *r = make_list();
    while (1) {
        ast_t *ast = read_decl_or_func_def();
        if (!ast)
            return r;
        list_push(r, ast);
    }
    list_free(globalenv->list);
    return r;
}
