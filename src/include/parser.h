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

#include "c_stackvm.h"
#include "dict.h"
#include "list.h"

#define MAX_ARGS    6
#define MAX_OP_PRIO 16
#define MAX_ALIGN   16

  Ast* ast_uop(int type, Ctype *ctype, Ast *operand);
  Ast* ast_binop(int type, Ast *left, Ast *right);
  Ast* ast_inttype(Ctype *ctype, long val);
  Ast* ast_double(double val);
 char* make_label(void);
  Ast* ast_lvar(Ctype *ctype, char *name);
  Ast* ast_gvar(Ctype *ctype, char *name, bool filelocal);
  Ast* ast_string(char *str);
  Ast* ast_funcall(Ctype *ctype, char *fname, List *args);
  Ast* ast_func(Ctype *rettype, char *fname, List *params, Ast *body, List *localvars);
  Ast* ast_decl(Ast *var, Ast *init);
  Ast* ast_array_init(List *arrayinit);
  Ast* ast_if(Ast *cond, Ast *then, Ast *els);
  Ast* ast_ternary(Ctype *ctype, Ast *cond, Ast *then, Ast *els);
  Ast* ast_for(Ast *init, Ast *cond, Ast *step, Ast *body);
  Ast* ast_return(Ast *retval);
  Ast* ast_compound_stmt(List *stmts);
  Ast* ast_struct_ref(Ctype *ctype, Ast *struc, char *name);
Ctype* make_ptr_type(Ctype *ctype);
Ctype* make_array_type(Ctype *ctype, int len);
Ctype* make_struct_field_type(Ctype *ctype, int offset);
Ctype* make_struct_type(Dict *fields, int size);
  bool is_inttype(Ctype *ctype);
  bool is_flotype(Ctype *ctype);
  void ensure_lvalue(Ast *ast);
  void expect(char punct);
  bool is_ident(const Token tok, char *s);
  bool is_right_assoc(const Token tok);
   int eval_intexpr(Ast *ast);
   int priority(const Token tok);
  Ast* read_func_args(char *fname);
  Ast* read_ident_or_func(char *name);
  bool is_long_token(char *p);
  bool is_int_token(char *p);
  bool is_float_token(char *p);
  Ast* read_prim(void);
Ctype* result_type_int(jmp_buf *jmpbuf, char op, Ctype *a, Ctype *b);
  Ast* read_subscript_expr(Ast *ast);
Ctype* convert_array(Ctype *ctype);
Ctype* result_type(char op, Ctype *a, Ctype *b);
  Ast* read_unary_expr(void);
  Ast* read_cond_expr(Ast *cond);
  Ast* read_struct_field(Ast *struc);
  Ast* read_expr_int(int prec);
  Ast* read_expr(void);
Ctype* get_ctype(const Token tok);
  bool is_type_keyword(const Token tok);
  Ast* read_decl_array_init_int(Ctype *ctype);
 char* read_struct_union_tag(void);
 Dict* read_struct_union_fields(void);
Ctype* read_union_def(void);
Ctype* read_struct_def(void);
Ctype* read_decl_spec(void);
  Ast* read_decl_init_val(Ast *var);
Ctype* read_array_dimensions_int(Ctype *basetype);
Ctype* read_array_dimensions(Ctype *basetype);
  Ast* read_decl_init(Ast *var);
Ctype* read_decl_int(Token *name);
  Ast* read_decl(void);
  Ast* read_if_stmt(void);
  Ast* read_opt_decl_or_stmt(void);
  Ast* read_opt_expr(void);
  Ast* read_for_stmt(void);
  Ast* read_return_stmt(void);
  Ast* read_stmt(void);
  Ast* read_decl_or_stmt(void);
  Ast* read_compound_stmt(void);
 List* read_params(void);
  Ast* read_func_def(Ctype *rettype, char *fname);
  Ast* read_decl_or_func_def(void);
 List* read_toplevels(void);

#endif /* PARSER_H_ */
