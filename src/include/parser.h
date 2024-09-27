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

  ast_t* ast_uop(int type, ctype_t *ctype, ast_t *operand);
  ast_t* ast_binop(int type, ast_t *left, ast_t *right);
  ast_t* ast_inttype(ctype_t *ctype, long val);
  ast_t* ast_double(double val);
   char* make_label(void);
  ast_t* ast_lvar(ctype_t *ctype, char *name);
  ast_t* ast_gvar(ctype_t *ctype, char *name, bool filelocal);
  ast_t* ast_string(char *str);
  ast_t* ast_funcall(ctype_t *ctype, char *fname, list_t *args);
  ast_t* ast_func(ctype_t *rettype, char *fname, list_t *params, ast_t *body, list_t *localvars);
  ast_t* ast_decl(ast_t *var, ast_t *init);
  ast_t* ast_array_init(list_t *arrayinit);
  ast_t* ast_if(ast_t *cond, ast_t *then, ast_t *els);
  ast_t* ast_ternary(ctype_t *ctype, ast_t *cond, ast_t *then, ast_t *els);
  ast_t* ast_for(ast_t *init, ast_t *cond, ast_t *step, ast_t *body);
  ast_t* ast_return(ast_t *retval);
  ast_t* ast_compound_stmt(list_t *stmts);
  ast_t* ast_struct_ref(ctype_t *ctype, ast_t *struc, char *name);
ctype_t* make_ptr_type(ctype_t *ctype);
ctype_t* make_array_type(ctype_t *ctype, int len);
ctype_t* make_struct_field_type(ctype_t *ctype, int offset);
ctype_t* make_struct_type(dict_t *fields, int size);
    bool is_inttype(ctype_t *ctype);
    bool is_flotype(ctype_t *ctype);
    void ensure_lvalue(ast_t *ast);
    void expect(char punct);
    bool is_ident(const token_t tok, char *s);
    bool is_right_assoc(const token_t tok);
     int eval_intexpr(ast_t *ast);
     int priority(const token_t tok);
  ast_t* read_func_args(char *fname);
  ast_t* read_ident_or_func(char *name);
    bool is_long_token(char *p);
    bool is_int_token(char *p);
    bool is_float_token(char *p);
  ast_t* read_prim(void);
ctype_t* result_type_int(jmp_buf *jmpbuf, char op, ctype_t *a, ctype_t *b);
  ast_t* read_subscript_expr(ast_t *ast);
ctype_t* convert_array(ctype_t *ctype);
ctype_t* result_type(char op, ctype_t *a, ctype_t *b);
  ast_t* read_unary_expr(void);
  ast_t* read_cond_expr(ast_t *cond);
  ast_t* read_struct_field(ast_t *struc);
  ast_t* read_expr_int(int prec);
  ast_t* read_expr(void);
ctype_t* get_ctype(const token_t tok);
    bool is_type_keyword(const token_t tok);
  ast_t* read_decl_array_init_int(ctype_t *ctype);
   char* read_struct_union_tag(void);
 dict_t* read_struct_union_fields(void);
ctype_t* read_union_def(void);
ctype_t* read_struct_def(void);
ctype_t* read_decl_spec(void);
  ast_t* read_decl_init_val(ast_t *var);
ctype_t* read_array_dimensions_int(ctype_t *basetype);
ctype_t* read_array_dimensions(ctype_t *basetype);
  ast_t* read_decl_init(ast_t *var);
ctype_t* read_decl_int(token_t *name);
  ast_t* read_decl(void);
  ast_t* read_if_stmt(void);
  ast_t* read_opt_decl_or_stmt(void);
  ast_t* read_opt_expr(void);
  ast_t* read_for_stmt(void);
  ast_t* read_return_stmt(void);
  ast_t* read_stmt(void);
  ast_t* read_decl_or_stmt(void);
  ast_t* read_compound_stmt(void);
 list_t* read_params(void);
  ast_t* read_func_def(ctype_t *rettype, char *fname);
  ast_t* read_decl_or_func_def(void);
 list_t* read_toplevels(void);

#endif /* PARSER_H_ */
