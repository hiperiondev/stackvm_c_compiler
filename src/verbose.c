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

#include "c_stackvm.h"

#define TAB_LEN 4

#define it tab+=TAB_LEN
#define dt tab-=TAB_LEN

int tab = 0;
bool cont = false;
bool excpt = false;
bool no_break = false;

char* ctype_to_string(Ctype *ctype) {
    if (!ctype)
        return "(nil)";

    switch (ctype->type) {
        case CTYPE_VOID:
            return "(VOID)";
        case CTYPE_INT:
            return "(INT)";
        case CTYPE_LONG:
            return "(LONG)";
        case CTYPE_CHAR:
            return "(CHAR)";
        case CTYPE_FLOAT:
            return "(FLOAT)";
#ifdef ALLOW_DOUBLE
        case CTYPE_DOUBLE:
            return "(DOUBLE)";
#endif
        case CTYPE_PTR: {
            String s = make_string();
            string_appendf(&s, "(PTR) %s", ctype_to_string(ctype->ptr));
            return get_cstring(s);
        }
        case CTYPE_ARRAY: {
            String s = make_string();
            string_appendf(&s, "(ARRAY) [%d] %s", ctype->len, ctype_to_string(ctype->ptr));
            return get_cstring(s);
        }
        case CTYPE_STRUCT: {
            String s = make_string();
            string_appendf(&s, "(STRUCT) ");
            for (Iter i = list_iter(dict_values(ctype->fields)); !iter_end(i);)
                string_appendf(&s, "%s ", ctype_to_string(iter_next(&i)));
            string_appendf(&s, "");
            return get_cstring(s);
        }
        default:
            error("Unknown ctype: %d", ctype);
            return NULL; /* non-reachable */
    }
}

static void uop_to_string(String *buf, char *op, Ast *ast) {
    char *aststr = ast_to_string(ast->operand, true);
    string_appendf(buf, "(%s %s)", op, aststr);
    lfree(aststr);
}

static void binop_to_string(String *buf, char *op, Ast *ast) {
    char *aststr1 = ast_to_string(ast->left, true);
    char *aststr2 = ast_to_string(ast->right, true);
    string_appendf(buf, "(%s %s %s)", op, aststr1, aststr2);
    lfree(aststr1);
    lfree(aststr2);
}

static void ast_to_string_int(String *buf, Ast *ast, bool first_entry) {
    if (!ast) {
        string_appendf(buf, "(nil)");
        return;
    }

    switch (ast->type) {
        case AST_LITERAL:
            string_appendf(buf, "(LITERAL) ");
            switch (ast->ctype->type) {
                case CTYPE_CHAR:
                    string_appendf(buf, "(CHAR) ");
                    if (ast->ival == '\n')
                        string_appendf(buf, "'\n'");
                    else if (ast->ival == '\\')
                        string_appendf(buf, "'\\\\'");
                    else
                        string_appendf(buf, "'%c'", ast->ival);
                    break;
                case CTYPE_INT:
                    string_appendf(buf, "(INT) ");
                    string_appendf(buf, "%d", ast->ival);
                    break;
                case CTYPE_LONG:
                    string_appendf(buf, "(LONG) ");
                    string_appendf(buf, "%ldL", ast->ival);
                    break;
                case CTYPE_FLOAT:
#ifdef ALLOW_DOUBLE
                case CTYPE_DOUBLE:
#endif
                    if (ast->type == CTYPE_FLOAT)
                        string_appendf(buf, "(FLOAT) ");
#ifdef ALLOW_DOUBLE
                    else
                        string_appendf(buf, "(DOUBLE) ");
#endif
                    string_appendf(buf, "%f", ast->fval);
                    break;
                default:
                    error("internal error");
            }
            break;
        case AST_STRING: {
            char *aststr = quote_cstring(ast->sval);
            string_appendf(buf, "(STRING) \"%s\"", aststr);
            lfree(aststr);
        }
            break;
        case AST_LVAR:
            string_appendf(buf, "(LVAR) %s", ast->varname);
            break;
        case AST_GVAR:
            string_appendf(buf, "(GVAR) %s", ast->varname);
            break;
        case AST_FUNCALL: {
            if(!excpt)
                string_appendf(buf, "%*s", tab, "");
            string_appendf(buf, "(FUNCALL) %s %s", ctype_to_string(ast->ctype), ast->fname);
            excpt = true;
            it;
            for (Iter i = list_iter(ast->args); !iter_end(i);) {
                string_appendf(buf, "\n");
                char *aststr = ast_to_string(iter_next(&i), true);
                string_appendf(buf, "%*s%s", tab, "", aststr);
                lfree(aststr);
            }
            dt;
            excpt = false;
            break;
        }
        case AST_FUNC: {
            string_appendf(buf, "%*s(FUNC) %s %s \n", tab, "", ctype_to_string(ast->ctype), ast->fname);
            it;
            for (Iter i = list_iter(ast->params); !iter_end(i);) {
                Ast *param = iter_next(&i);
                char *aststr4 = ast_to_string(param, true);
                string_appendf(buf, "%s %s\n", ctype_to_string(param->ctype), aststr4);
                lfree(aststr4);
            }

            ast_to_string_int(buf, ast->body, false);
            dt;
            break;
        }
        case AST_DECL: {
            if (!excpt)
                string_appendf(buf, "%*s", tab, "");
            string_appendf(buf, "(DECL) %s %s", ctype_to_string(ast->declvar->ctype), ast->declvar->varname);
            if (ast->declinit) {
                it;
                char *aststr = ast_to_string(ast->declinit, true);
                string_appendf(buf, " %s", aststr);
                lfree(aststr);
                dt;
            }
        }
            break;
        case AST_ARRAY_INIT:
            string_appendf(buf, "\n%*s(ARRAY_INIT)\n", tab, "");
            it;
            for (Iter i = list_iter(ast->arrayinit); !iter_end(i);) {
                string_appendf(buf, "%*s", tab, "");
                ast_to_string_int(buf, iter_next(&i), false);
                string_appendf(buf, "\n");
            }
            dt;
            string_appendf(buf, "%*s", tab - TAB_LEN, "");
            break;
        case AST_IF: {
            string_appendf(buf, "%*s(IF)\n", tab, "");
            it;
            cont = true;
            excpt = true;
            char *aststr = ast_to_string(ast->cond, true);
            string_appendf(buf, "%*s(CONDITION) %s\n", tab, "", aststr);
            lfree(aststr);
            excpt = false;
            it;
            aststr = ast_to_string(ast->then, true);
            string_appendf(buf, "%s", aststr);
            lfree(aststr);
            dt;
            if (ast->els) {
                string_appendf(buf, "\n");
                string_appendf(buf, "%*s(ELSE)\n", tab, "");
                dt;
                string_appendf(buf, "%*s%s", tab, "", ast_to_string(ast->els, true));
                it;
            }
            dt;
        }
            break;
        case AST_TERNARY: {
            char *aststr1 = ast_to_string(ast->cond, true);
            char *aststr2 = ast_to_string(ast->then, true);
            char *aststr3 = ast_to_string(ast->els, true);
            string_appendf(buf, "(? %s %s %s)", aststr1, aststr2, aststr3);
            lfree(aststr1);
            lfree(aststr2);
            lfree(aststr3);
        }
            break;
        case AST_FOR: {
            excpt = true;

            char *aststr1 = ast_to_string(ast->forinit, true);
            char *aststr2 = ast_to_string(ast->forcond, true);
            char *aststr3 = ast_to_string(ast->forstep, true);
            string_appendf(buf, "%*s(FOR %s %s %s) \n", tab, "", aststr1, aststr2, aststr3);
            lfree(aststr1);
            lfree(aststr2);
            lfree(aststr3);

            excpt = false;
            it;
            no_break = true;
            char *aststr4 = ast_to_string(ast->forbody, false);
            string_appendf(buf, "%s", aststr4);
            lfree(aststr4);
            no_break = false;
            dt;
        }
            break;
        case AST_RETURN: {
            char *aststr = ast_to_string(ast->retval, true);
            string_appendf(buf, "%*s(RETURN) %s", tab, "", aststr);
            lfree(aststr);
        }
            break;
        case AST_COMPOUND_STMT: {
            string_appendf(buf, "%*s(COMPOUND_STMT)", tab, "");
            it;
            no_break = true;
            for (Iter i = list_iter(ast->stmts); !iter_end(i);) {
                cont = false;
                string_appendf(buf, "\n");
                ast_to_string_int(buf, iter_next(&i), false);
            }
            no_break = false;
            dt;
            break;
        }
        case AST_STRUCT_REF:
            ast_to_string_int(buf, ast->struc, false);
            string_appendf(buf, ".");
            string_appendf(buf, ast->field);
            break;
        case AST_ADDR:
            uop_to_string(buf, "(ADDR)", ast);
            break;
        case AST_DEREF:
            uop_to_string(buf, "(DEREF)", ast);
            break;
        case PUNCT_INC:
            uop_to_string(buf, "++", ast);
            break;
        case PUNCT_DEC:
            uop_to_string(buf, "--", ast);
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
                string_appendf(buf, "%*s", tab, "");
            }

            if (ast->type == PUNCT_EQ) {
                string_appendf(buf, "(== ");
            } else {
                string_appendf(buf, "(%c", ast->type);
                if (ast->type < 127)
                    string_appendf(buf, " ");
            }

            string_appendf(buf, "%s %s)", left, right);

            if (!no_break && !(cont || first_entry)) {
                string_appendf(buf, "\n");
            }

            lfree(left);
            lfree(right);
    }
}

char* ast_to_string(Ast *ast, bool first_entry) {
    String s = make_string();
    ast_to_string_int(&s, ast, first_entry);
    return get_cstring(s);
}

char* token_to_string(const Token tok) {
    enum TokenType ttype = get_ttype(tok);
    if (ttype == TTYPE_NULL)
        return "(null)";
    String s = make_string();
    switch (ttype) {
        case TTYPE_NULL:
            error("internal error: unknown token type: %d", get_ttype(tok));
        case TTYPE_IDENT:
            return get_ident(tok);
        case TTYPE_PUNCT:
            if (is_punct(tok, PUNCT_EQ))
                string_appendf(&s, "==");
            else
                string_appendf(&s, "%c", get_punct(tok));
            return get_cstring(s);
        case TTYPE_CHAR:
            string_append(&s, get_char(tok));
            return get_cstring(s);
        case TTYPE_NUMBER:
            return get_number(tok);
        case TTYPE_STRING:
            string_appendf(&s, "\"%s\"", get_strtok(tok));
            return get_cstring(s);
    }
    error("internal error: unknown token type: %d", get_ttype(tok));
    return NULL; /* non-reachable */
}
