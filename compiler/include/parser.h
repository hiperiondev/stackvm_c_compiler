/*
 * @parser.h
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

#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include <setjmp.h>

#include "c_stackvm.h"
#include "dict.h"
#include "list.h"

/**
 * @def MAX_ARGS
 * @brief
 *
 */
#define MAX_ARGS    6

/**
 * @def MAX_OP_PRIO
 * @brief
 *
 */
#define MAX_OP_PRIO 16

/**
 * @def MAX_ALIGN
 * @brief
 *
 */
#define MAX_ALIGN   16

/**
 * @fn ast_t parser_ast_uop*(int, ctype_t*, ast_t*)
 * @brief
 *
 * @param type
 * @param ctype
 * @param operand
 * @return
 */
ast_t* parser_ast_uop(int type, ctype_t *ctype, ast_t *operand);

/**
 * @fn ast_t parser_ast_binop*(int, ast_t*, ast_t*)
 * @brief
 *
 * @param type
 * @param left
 * @param right
 * @return
 */
ast_t* parser_ast_binop(int type, ast_t *left, ast_t *right);

/**
 * @fn ast_t parser_ast_inttype*(ctype_t*, long)
 * @brief
 *
 * @param ctype
 * @param val
 * @return
 */
ast_t* parser_ast_inttype(ctype_t *ctype, long val);

/**
 * @fn ast_t parser_ast_double*(double)
 * @brief
 *
 * @param val
 * @return
 */
ast_t* parser_ast_double(double val);

/**
 * @fn char parser_make_label*(void)
 * @brief
 *
 * @return
 */
char* parser_make_label(void);

/**
 * @fn ast_t parser_ast_lvar*(ctype_t*, char*)
 * @brief
 *
 * @param ctype
 * @param name
 * @return
 */
ast_t* parser_ast_lvar(ctype_t *ctype, char *name);

/**
 * @fn ast_t parser_ast_gvar*(ctype_t*, char*, bool)
 * @brief
 *
 * @param ctype
 * @param name
 * @param filelocal
 * @return
 */
ast_t* parser_ast_gvar(ctype_t *ctype, char *name, bool filelocal);

/**
 * @fn ast_t parser_ast_string*(char*)
 * @brief
 *
 * @param str
 * @return
 */
ast_t* parser_ast_string(char *str);

/**
 * @fn ast_t parser_ast_funcall*(ctype_t*, char*, list_t*)
 * @brief
 *
 * @param ctype
 * @param fname
 * @param args
 * @return
 */
ast_t* parser_ast_funcall(ctype_t *ctype, char *fname, list_t *args);

/**
 * @fn ast_t parser_ast_func*(ctype_t*, char*, list_t*, ast_t*, list_t*)
 * @brief
 *
 * @param rettype
 * @param fname
 * @param params
 * @param body
 * @param localvars
 * @return
 */
ast_t* parser_ast_func(ctype_t *rettype, char *fname, list_t *params, ast_t *body, list_t *localvars);

/**
 * @fn ast_t parser_ast_decl*(ast_t*, ast_t*)
 * @brief
 *
 * @param var
 * @param init
 * @return
 */
ast_t* parser_ast_decl(ast_t *var, ast_t *init);

/**
 * @fn ast_t parser_ast_array_init*(list_t*)
 * @brief
 *
 * @param arrayinit
 * @return
 */
ast_t* parser_ast_array_init(list_t *arrayinit);

/**
 * @fn ast_t parser_ast_if*(ast_t*, ast_t*, ast_t*)
 * @brief
 *
 * @param cond
 * @param then
 * @param els
 * @return
 */
ast_t* parser_ast_if(ast_t *cond, ast_t *then, ast_t *els);

/**
 * @fn ast_t parser_ast_ternary*(ctype_t*, ast_t*, ast_t*, ast_t*)
 * @brief
 *
 * @param ctype
 * @param cond
 * @param then
 * @param els
 * @return
 */
ast_t* parser_ast_ternary(ctype_t *ctype, ast_t *cond, ast_t *then, ast_t *els);

/**
 * @fn ast_t parser_ast_for*(ast_t*, ast_t*, ast_t*, ast_t*)
 * @brief
 *
 * @param init
 * @param cond
 * @param step
 * @param body
 * @return
 */
ast_t* parser_ast_for(ast_t *init, ast_t *cond, ast_t *step, ast_t *body);

/**
 * @fn ast_t parser_ast_return*(ast_t*)
 * @brief
 *
 * @param retval
 * @return
 */
ast_t* parser_ast_return(ast_t *retval);

/**
 * @fn ast_t parser_ast_compound_stmt*(list_t*)
 * @brief
 *
 * @param stmts
 * @return
 */
ast_t* parser_ast_compound_stmt(list_t *stmts);

/**
 * @fn ast_t parser_ast_struct_ref*(ctype_t*, ast_t*, char*)
 * @brief
 *
 * @param ctype
 * @param struc
 * @param name
 * @return
 */
ast_t* parser_ast_struct_ref(ctype_t *ctype, ast_t *struc, char *name);

/**
 * @fn ctype_t parser_make_ptr_type*(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
ctype_t* parser_make_ptr_type(ctype_t *ctype);

/**
 * @fn ctype_t parser_make_array_type*(ctype_t*, int)
 * @brief
 *
 * @param ctype
 * @param len
 * @return
 */
ctype_t* parser_make_array_type(ctype_t *ctype, int len);

/**
 * @fn ctype_t parser_make_struct_field_type*(ctype_t*, int)
 * @brief
 *
 * @param ctype
 * @param offset
 * @return
 */
ctype_t* parser_make_struct_field_type(ctype_t *ctype, int offset);

/**
 * @fn ctype_t parser_make_struct_type*(dict_t*, int)
 * @brief
 *
 * @param fields
 * @param size
 * @return
 */
ctype_t* parser_make_struct_type(dict_t *fields, int size);

/**
 * @fn bool parser_is_inttype(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
bool parser_is_inttype(ctype_t *ctype);

/**
 * @fn bool parser_is_flotype(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
bool parser_is_flotype(ctype_t *ctype);

/**
 * @fn void parser_ensure_lvalue(ast_t*)
 * @brief
 *
 * @param ast
 */
void parser_ensure_lvalue(ast_t *ast);

/**
 * @fn void parser_expect(char)
 * @brief
 *
 * @param punct
 */
void parser_expect(char punct);

/**
 * @fn bool parser_is_ident(const token_t, char*)
 * @brief
 *
 * @param tok
 * @param s
 * @return
 */
bool parser_is_ident(const token_t tok, char *s);

/**
 * @fn bool parser_is_right_assoc(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
bool parser_is_right_assoc(const token_t tok);

/**
 * @fn int parser_eval_intexpr(ast_t*)
 * @brief
 *
 * @param ast
 * @return
 */
int parser_eval_intexpr(ast_t *ast);

/**
 * @fn int parser_priority(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
int parser_priority(const token_t tok);

/**
 * @fn ast_t parser_read_func_args*(char*)
 * @brief
 *
 * @param fname
 * @return
 */
ast_t* parser_read_func_args(char *fname);

/**
 * @fn ast_t parser_read_ident_or_func*(char*)
 * @brief
 *
 * @param name
 * @return
 */
ast_t* parser_read_ident_or_func(char *name);

/**
 * @fn bool parser_is_long_token(char*)
 * @brief
 *
 * @param p
 * @return
 */
bool parser_is_long_token(char *p);

/**
 * @fn bool parser_is_int_token(char*)
 * @brief
 *
 * @param p
 * @return
 */
bool parser_is_int_token(char *p);

/**
 * @fn bool parser_is_float_token(char*)
 * @brief
 *
 * @param p
 * @return
 */
bool parser_is_float_token(char *p);

/**
 * @fn ast_t parser_read_prim*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_prim(void);

/**
 * @fn ctype_t parser_result_type_int*(jmp_buf*, char, ctype_t*, ctype_t*)
 * @brief
 *
 * @param jmpbuf
 * @param op
 * @param a
 * @param b
 * @return
 */
ctype_t* parser_result_type_int(jmp_buf *jmpbuf, char op, ctype_t *a, ctype_t *b);

/**
 * @fn ast_t parser_read_subscript_expr*(ast_t*)
 * @brief
 *
 * @param ast
 * @return
 */
ast_t* parser_read_subscript_expr(ast_t *ast);

/**
 * @fn ctype_t parser_convert_array*(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
ctype_t* parser_convert_array(ctype_t *ctype);

/**
 * @fn ctype_t parser_result_type*(char, ctype_t*, ctype_t*)
 * @brief
 *
 * @param op
 * @param a
 * @param b
 * @return
 */
ctype_t* parser_result_type(char op, ctype_t *a, ctype_t *b);

/**
 * @fn ast_t parser_read_unary_expr*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_unary_expr(void);

/**
 * @fn ast_t parser_read_cond_expr*(ast_t*)
 * @brief
 *
 * @param cond
 * @return
 */
ast_t* parser_read_cond_expr(ast_t *cond);

/**
 * @fn ast_t parser_read_struct_field*(ast_t*)
 * @brief
 *
 * @param struc
 * @return
 */
ast_t* parser_read_struct_field(ast_t *struc);

/**
 * @fn ast_t parser_read_expr_int*(int)
 * @brief
 *
 * @param prec
 * @return
 */
ast_t* parser_read_expr_int(int prec);

/**
 * @fn ast_t parser_read_expr*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_expr(void);

/**
 * @fn ctype_t parser_get_ctype*(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
ctype_t* parser_get_ctype(const token_t tok);

/**
 * @fn bool parser_is_type_keyword(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
bool parser_is_type_keyword(const token_t tok);

/**
 * @fn ast_t parser_read_decl_array_init_int*(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
ast_t* parser_read_decl_array_init_int(ctype_t *ctype);

/**
 * @fn char parser_read_struct_union_tag*(void)
 * @brief
 *
 * @return
 */
char* parser_read_struct_union_tag(void);

/**
 * @fn dict_t parser_read_struct_union_fields*(void)
 * @brief
 *
 * @return
 */
dict_t* parser_read_struct_union_fields(void);

/**
 * @fn ctype_t parser_read_union_def*(void)
 * @brief
 *
 * @return
 */
ctype_t* parser_read_union_def(void);

/**
 * @fn ctype_t parser_read_struct_def*(void)
 * @brief
 *
 * @return
 */
ctype_t* parser_read_struct_def(void);

/**
 * @fn ctype_t parser_read_decl_spec*(void)
 * @brief
 *
 * @return
 */
ctype_t* parser_read_decl_spec(void);

/**
 * @fn ast_t parser_read_decl_init_val*(ast_t*)
 * @brief
 *
 * @param var
 * @return
 */
ast_t* parser_read_decl_init_val(ast_t *var);

/**
 * @fn ctype_t parser_read_array_dimensions_int*(ctype_t*)
 * @brief
 *
 * @param basetype
 * @return
 */
ctype_t* parser_read_array_dimensions_int(ctype_t *basetype);

/**
 * @fn ctype_t parser_read_array_dimensions*(ctype_t*)
 * @brief
 *
 * @param basetype
 * @return
 */
ctype_t* parser_read_array_dimensions(ctype_t *basetype);

/**
 * @fn ast_t parser_read_decl_init*(ast_t*)
 * @brief
 *
 * @param var
 * @return
 */
ast_t* parser_read_decl_init(ast_t *var);

/**
 * @fn ctype_t parser_read_decl_int*(token_t*)
 * @brief
 *
 * @param name
 * @return
 */
ctype_t* parser_read_decl_int(token_t *name);

/**
 * @fn ast_t parser_read_decl*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_decl(void);

/**
 * @fn ast_t parser_read_if_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_if_stmt(void);

/**
 * @fn ast_t parser_read_opt_decl_or_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_opt_decl_or_stmt(void);

/**
 * @fn ast_t parser_read_opt_expr*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_opt_expr(void);

/**
 * @fn ast_t parser_read_for_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_for_stmt(void);

/**
 * @fn ast_t parser_read_return_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_return_stmt(void);

/**
 * @fn ast_t parser_read_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_stmt(void);

/**
 * @fn ast_t parser_read_decl_or_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_decl_or_stmt(void);

/**
 * @fn ast_t parser_read_compound_stmt*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_compound_stmt(void);

/**
 * @fn list_t parser_read_params*(void)
 * @brief
 *
 * @return
 */
list_t* parser_read_params(void);

/**
 * @fn ast_t parser_read_func_def*(ctype_t*, char*)
 * @brief
 *
 * @param rettype
 * @param fname
 * @return
 */
ast_t* parser_read_func_def(ctype_t *rettype, char *fname);

/**
 * @fn ast_t parser_read_decl_or_func_def*(void)
 * @brief
 *
 * @return
 */
ast_t* parser_read_decl_or_func_def(void);

/**
 * @fn list_t parser_read_toplevels*(void)
 * @brief
 *
 * @return
 */
list_t* parser_read_toplevels(void);

#endif /* PARSER_H_ */
