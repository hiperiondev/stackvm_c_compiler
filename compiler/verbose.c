/*
 * @verbose.c
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "c_stackvm.h"
#include "verbose.h"
#include "lexer.h"

static int tab = 0;
static bool cont = false;
static bool excpt = false;
static bool no_break = false;

char* ctype_to_string(ctype_t *ctype) {
    if (!ctype)
        return "(nil)";

    switch (ctype->type) {
        case CTYPE_VOID:
            return "(VOID)";
        case CTYPE_INT:
            return "(INT)";
        case CTYPE_UINT:
            return "(UINT)";
#ifdef ALLOW_LONG
        case CTYPE_LONG:
            return "(LONG)";
#endif
        case CTYPE_CHAR:
            return "(CHAR)";
        case CTYPE_FLOAT:
            return "(FLOAT)";
#ifdef ALLOW_DOUBLE
        case CTYPE_DOUBLE:
            return "(DOUBLE)";
#endif
        case CTYPE_PTR: {
            string_t s = util_make_string();
            util_string_appendf(&s, "(PTR) %s", ctype_to_string(ctype->ptr));
            return util_get_cstring(s);
        }
        case CTYPE_ARRAY: {
            string_t s = util_make_string();
            util_string_appendf(&s, "(ARRAY) [%d] %s", ctype->len, ctype_to_string(ctype->ptr));
            return util_get_cstring(s);
        }
        case CTYPE_STRUCT: {
            string_t s = util_make_string();
            util_string_appendf(&s, "(STRUCT) ");
            for (iter_t i = list_iter(dict_values(ctype->fields)); !list_iter_end(i);)
                util_string_appendf(&s, "%s ", ctype_to_string(list_iter_next(&i)));
            util_string_appendf(&s, "");
            return util_get_cstring(s);
        }
        default:
            util_error("Unknown ctype: %d", ctype);
            return NULL; /* non-reachable */
    }
}

void uop_to_string(string_t *buf, char *op, ast_t *ast) {
    char *aststr = ast_to_string(ast->operand, true);
    util_string_appendf(buf, "(%s %s)", op, aststr);
    util_lfree(aststr);
}

void binop_to_string(string_t *buf, char *op, ast_t *ast) {
    char *aststr1 = ast_to_string(ast->left, true);
    char *aststr2 = ast_to_string(ast->right, true);
    util_string_appendf(buf, "(%s %s %s)", op, aststr1, aststr2);
    util_lfree(aststr1);
    util_lfree(aststr2);
}

void ast_to_string_int(string_t *buf, ast_t *ast, bool first_entry) {
    if (!ast) {
        util_string_appendf(buf, "(nil)");
        return;
    }

    switch (ast->type) {
        case AST_LITERAL:
            util_string_appendf(buf, "(LITERAL) ");
            switch (ast->ctype->type) {
                case CTYPE_CHAR:
                    util_string_appendf(buf, "(CHAR) ");
                    if (ast->ival == '\n')
                        util_string_appendf(buf, "'\n'");
                    else if (ast->ival == '\\')
                        util_string_appendf(buf, "'\\\\'");
                    else
                        util_string_appendf(buf, "'%c'", ast->ival);
                    break;
                case CTYPE_INT:
                    util_string_appendf(buf, "(INT) ");
                    util_string_appendf(buf, "%d", ast->ival);
                    break;
                case CTYPE_UINT:
                    util_string_appendf(buf, "(UINT) ");
                    util_string_appendf(buf, "%d", ast->ival);
                    break;
#ifdef ALLOW_LONG
                case CTYPE_LONG:
                    util_string_appendf(buf, "(LONG) ");
                    util_string_appendf(buf, "%ldL", ast->ival);
                    break;
#endif
                case CTYPE_FLOAT:
#ifdef ALLOW_DOUBLE
                case CTYPE_DOUBLE:
#endif
                    if (ast->type == CTYPE_FLOAT)
                        util_string_appendf(buf, "(FLOAT) ");
#ifdef ALLOW_DOUBLE
                    else
                        util_string_appendf(buf, "(DOUBLE) ");
#endif
                    util_string_appendf(buf, "%f", ast->fval);
                    break;
                default:
                    util_error("internal error");
            }
            break;
        case AST_STRING: {
            char *aststr = util_quote_cstring(ast->sval);
            util_string_appendf(buf, "(STRING) \"%s\"", aststr);
            util_lfree(aststr);
        }
            break;
        case AST_LVAR:
            util_string_appendf(buf, "(LVAR) %s", ast->varname);
            break;
        case AST_GVAR:
            util_string_appendf(buf, "(GVAR) %s", ast->varname);
            break;
        case AST_FUNCALL: {
            if(!excpt)
                util_string_appendf(buf, "%*s", tab, "");
            util_string_appendf(buf, "(FUNCALL) %s %s", ctype_to_string(ast->ctype), ast->fname);
            excpt = true;
            it;
            for (iter_t i = list_iter(ast->args); !list_iter_end(i);) {
                util_string_appendf(buf, "\n");
                char *aststr = ast_to_string(list_iter_next(&i), true);
                util_string_appendf(buf, "%*s%s", tab, "", aststr);
                util_lfree(aststr);
            }
            dt;
            excpt = false;
            break;
        }
        case AST_FUNC: {
            util_string_appendf(buf, "%*s(FUNC) %s %s \n", tab, "", ctype_to_string(ast->ctype), ast->fname);
            it;
            for (iter_t i = list_iter(ast->params); !list_iter_end(i);) {
                ast_t *param = list_iter_next(&i);
                char *aststr4 = ast_to_string(param, true);
                util_string_appendf(buf, "%s %s\n", ctype_to_string(param->ctype), aststr4);
                util_lfree(aststr4);
            }

            ast_to_string_int(buf, ast->body, false);
            dt;
            break;
        }
        case AST_DECL: {
            if (!excpt)
                util_string_appendf(buf, "%*s", tab, "");
            util_string_appendf(buf, "(DECL) %s %s", ctype_to_string(ast->declvar->ctype), ast->declvar->varname);
            if (ast->declinit) {
                it;
                char *aststr = ast_to_string(ast->declinit, true);
                util_string_appendf(buf, " %s", aststr);
                util_lfree(aststr);
                dt;
            }
        }
            break;
        case AST_ARRAY_INIT:
            util_string_appendf(buf, "\n%*s(ARRAY_INIT)\n", tab, "");
            it;
            for (iter_t i = list_iter(ast->arrayinit); !list_iter_end(i);) {
                util_string_appendf(buf, "%*s", tab, "");
                ast_to_string_int(buf, list_iter_next(&i), false);
                util_string_appendf(buf, "\n");
            }
            dt;
            util_string_appendf(buf, "%*s", tab - TAB_LEN, "");
            break;
        case AST_IF: {
            util_string_appendf(buf, "%*s(IF)\n", tab, "");
            it;
            cont = true;
            excpt = true;
            char *aststr = ast_to_string(ast->cond, true);
            util_string_appendf(buf, "%*s(CONDITION) %s\n", tab, "", aststr);
            util_lfree(aststr);
            excpt = false;
            it;
            aststr = ast_to_string(ast->then, true);
            util_string_appendf(buf, "%s", aststr);
            util_lfree(aststr);
            dt;
            if (ast->els) {
                util_string_appendf(buf, "\n");
                util_string_appendf(buf, "%*s(ELSE)\n", tab, "");
                dt;
                util_string_appendf(buf, "%*s%s", tab, "", ast_to_string(ast->els, true));
                it;
            }
            dt;
        }
            break;
        case AST_TERNARY: {
            char *aststr1 = ast_to_string(ast->cond, true);
            char *aststr2 = ast_to_string(ast->then, true);
            char *aststr3 = ast_to_string(ast->els, true);
            util_string_appendf(buf, "(? %s %s %s)", aststr1, aststr2, aststr3);
            util_lfree(aststr1);
            util_lfree(aststr2);
            util_lfree(aststr3);
        }
            break;
        case AST_FOR: {
            excpt = true;

            char *aststr1 = ast_to_string(ast->forinit, true);
            char *aststr2 = ast_to_string(ast->forcond, true);
            char *aststr3 = ast_to_string(ast->forstep, true);
            util_string_appendf(buf, "%*s(FOR %s %s %s) \n", tab, "", aststr1, aststr2, aststr3);
            util_lfree(aststr1);
            util_lfree(aststr2);
            util_lfree(aststr3);

            excpt = false;
            it;
            no_break = true;
            char *aststr4 = ast_to_string(ast->forbody, false);
            util_string_appendf(buf, "%s", aststr4);
            util_lfree(aststr4);
            no_break = false;
            dt;
        }
            break;
        case AST_RETURN: {
            char *aststr = ast_to_string(ast->retval, true);
            util_string_appendf(buf, "%*s(RETURN) %s", tab, "", aststr);
            util_lfree(aststr);
        }
            break;
        case AST_COMPOUND_STMT: {
            util_string_appendf(buf, "%*s(COMPOUND_STMT)", tab, "");
            it;
            no_break = true;
            for (iter_t i = list_iter(ast->stmts); !list_iter_end(i);) {
                cont = false;
                util_string_appendf(buf, "\n");
                ast_to_string_int(buf, list_iter_next(&i), false);
            }
            no_break = false;
            dt;
            break;
        }
        case AST_STRUCT_REF:
            ast_to_string_int(buf, ast->struc, false);
            util_string_appendf(buf, ".");
            util_string_appendf(buf, ast->field);
            break;
        case AST_ADDR:
            uop_to_string(buf, "(ADDR)", ast);
            break;
        case AST_DEREF:
            uop_to_string(buf, "(DEREF)", ast);
            break;
        case PUNCT_POSTINC:
            if (!(cont || first_entry)) {
                util_string_appendf(buf, "%*s", tab, "");
            }
            uop_to_string(buf, "POSTINC", ast);
            break;
        case PUNCT_PREINC:
            if (!(cont || first_entry)) {
                util_string_appendf(buf, "%*s", tab, "");
            }
            uop_to_string(buf, "PREINC", ast);
            break;
        case PUNCT_POSTDEC:
            if (!(cont || first_entry)) {
                util_string_appendf(buf, "%*s", tab, "");
            }
            uop_to_string(buf, "POSTDEC", ast);
            break;
        case PUNCT_PREDEC:
            if (!(cont || first_entry)) {
                util_string_appendf(buf, "%*s", tab, "");
            }
            uop_to_string(buf, "PREDEC", ast);
            break;
        case PUNCT_LOGAND:
            binop_to_string(buf, "and", ast);
            break;
        case PUNCT_LOGOR:
            binop_to_string(buf, "or", ast);
            break;
        case PUNCT_LSHIFT:
            binop_to_string(buf, "<<", ast);
            break;
        case PUNCT_RSHIFT:
            binop_to_string(buf, ">>", ast);
            break;
        case '!':
            uop_to_string(buf, "!", ast);
            cont = true;
            break;
        case '&':
            binop_to_string(buf, "&", ast);
            break;
        case '|':
            binop_to_string(buf, "|", ast);
            break;
        default:
            char *left = ast_to_string(ast->left, true);
            char *right = ast_to_string(ast->right, true);

            if (!(cont || first_entry)) {
                util_string_appendf(buf, "%*s", tab, "");
            }

            if (ast->type == PUNCT_EQ) {
                util_string_appendf(buf, "(== ");
            } else {
                util_string_appendf(buf, "(%c", ast->type);
                if (ast->type < 127)
                    util_string_appendf(buf, " ");
            }

            util_string_appendf(buf, "%s %s)", left, right);

            if (!no_break && !(cont || first_entry)) {
                util_string_appendf(buf, "\n");
            }

            util_lfree(left);
            util_lfree(right);
    }
}

char* ast_to_string(ast_t *ast, bool first_entry) {
    string_t s = util_make_string();
    ast_to_string_int(&s, ast, first_entry);
    return util_get_cstring(s);
}

char* token_to_string(const token_t tok) {
    enum token_type ttype = get_ttype(tok);

    if (ttype == TTYPE_NULL)
        return "(null)";
    string_t s = util_make_string();
    switch (ttype) {
        case TTYPE_NULL:
            util_error("internal error: unknown token type: %d", get_ttype(tok));
            break;
        case TTYPE_IDENT:
            return get_ident(tok);
        case TTYPE_PUNCT:
            if (lexer_is_punct(tok, PUNCT_EQ))
                util_string_appendf(&s, "==");
            else
                util_string_appendf(&s, "%c", get_punct(tok));
            return util_get_cstring(s);
        case TTYPE_CHAR:
            util_string_append(&s, get_char(tok));
            return util_get_cstring(s);
        case TTYPE_NUMBER:
            return get_number(tok);
        case TTYPE_STRING:
            util_string_appendf(&s, "\"%s\"", get_strtok(tok));
            return util_get_cstring(s);
    }

    util_error("internal error: unknown token type: %d", get_ttype(tok));
    return NULL; /* non-reachable */
}
