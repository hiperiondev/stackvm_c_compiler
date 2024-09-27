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

#define MAX_ARGS    6
#define MAX_OP_PRIO 16
#define MAX_ALIGN   16

  ast_t* parser_ast_uop(int type, ctype_t *ctype, ast_t *operand);
  ast_t* parser_ast_binop(int type, ast_t *left, ast_t *right);
  ast_t* parser_ast_inttype(ctype_t *ctype, long val);
  ast_t* parser_ast_double(double val);
   char* parser_make_label(void);
  ast_t* parser_ast_lvar(ctype_t *ctype, char *name);
  ast_t* parser_ast_gvar(ctype_t *ctype, char *name, bool filelocal);
  ast_t* parser_ast_string(char *str);
  ast_t* parser_ast_funcall(ctype_t *ctype, char *fname, list_t *args);
  ast_t* parser_ast_func(ctype_t *rettype, char *fname, list_t *params, ast_t *body, list_t *localvars);
  ast_t* parser_ast_decl(ast_t *var, ast_t *init);
  ast_t* parser_ast_array_init(list_t *arrayinit);
  ast_t* parser_ast_if(ast_t *cond, ast_t *then, ast_t *els);
  ast_t* parser_ast_ternary(ctype_t *ctype, ast_t *cond, ast_t *then, ast_t *els);
  ast_t* parser_ast_for(ast_t *init, ast_t *cond, ast_t *step, ast_t *body);
  ast_t* parser_ast_return(ast_t *retval);
  ast_t* parser_ast_compound_stmt(list_t *stmts);
  ast_t* parser_ast_struct_ref(ctype_t *ctype, ast_t *struc, char *name);
ctype_t* parser_make_ptr_type(ctype_t *ctype);
ctype_t* parser_make_array_type(ctype_t *ctype, int len);
ctype_t* parser_make_struct_field_type(ctype_t *ctype, int offset);
ctype_t* parser_make_struct_type(dict_t *fields, int size);
    bool parser_is_inttype(ctype_t *ctype);
    bool parser_is_flotype(ctype_t *ctype);
    void parser_ensure_lvalue(ast_t *ast);
    void parser_expect(char punct);
    bool parser_is_ident(const token_t tok, char *s);
    bool parser_is_right_assoc(const token_t tok);
     int parser_eval_intexpr(ast_t *ast);
     int parser_priority(const token_t tok);
  ast_t* parser_read_func_args(char *fname);
  ast_t* parser_read_ident_or_func(char *name);
    bool parser_is_long_token(char *p);
    bool parser_is_int_token(char *p);
    bool parser_is_float_token(char *p);
  ast_t* parser_read_prim(void);
ctype_t* parser_result_type_int(jmp_buf *jmpbuf, char op, ctype_t *a, ctype_t *b);
  ast_t* parser_read_subscript_expr(ast_t *ast);
ctype_t* parser_convert_array(ctype_t *ctype);
ctype_t* parser_result_type(char op, ctype_t *a, ctype_t *b);
  ast_t* parser_read_unary_expr(void);
  ast_t* parser_read_cond_expr(ast_t *cond);
  ast_t* parser_read_struct_field(ast_t *struc);
  ast_t* parser_read_expr_int(int prec);
  ast_t* parser_read_expr(void);
ctype_t* parser_get_ctype(const token_t tok);
    bool parser_is_type_keyword(const token_t tok);
  ast_t* parser_read_decl_array_init_int(ctype_t *ctype);
   char* parser_read_struct_union_tag(void);
 dict_t* parser_read_struct_union_fields(void);
ctype_t* parser_read_union_def(void);
ctype_t* parser_read_struct_def(void);
ctype_t* parser_read_decl_spec(void);
  ast_t* parser_read_decl_init_val(ast_t *var);
ctype_t* parser_read_array_dimensions_int(ctype_t *basetype);
ctype_t* parser_read_array_dimensions(ctype_t *basetype);
  ast_t* parser_read_decl_init(ast_t *var);
ctype_t* parser_read_decl_int(token_t *name);
  ast_t* parser_read_decl(void);
  ast_t* parser_read_if_stmt(void);
  ast_t* parser_read_opt_decl_or_stmt(void);
  ast_t* parser_read_opt_expr(void);
  ast_t* parser_read_for_stmt(void);
  ast_t* parser_read_return_stmt(void);
  ast_t* parser_read_stmt(void);
  ast_t* parser_read_decl_or_stmt(void);
  ast_t* parser_read_compound_stmt(void);
 list_t* parser_read_params(void);
  ast_t* parser_read_func_def(ctype_t *rettype, char *fname);
  ast_t* parser_read_decl_or_func_def(void);
 list_t* parser_read_toplevels(void);

#endif /* PARSER_H_ */
