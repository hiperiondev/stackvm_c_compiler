/*
 * @verbose.h
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

#ifndef VERBOSE_H_
#define VERBOSE_H_

#define TAB_LEN 4
#define it tab+=TAB_LEN
#define dt tab-=TAB_LEN

char* ctype_to_string(ctype_t *ctype);
 void uop_to_string(string_t *buf, char *op, ast_t *ast);
 void binop_to_string(string_t *buf, char *op, ast_t *ast);
 void ast_to_string_int(string_t *buf, ast_t *ast, bool first_entry);
char* ast_to_string(ast_t *ast, bool first_entry);
char* token_to_string(const token_t tok);

#endif /* VERBOSE_H_ */
