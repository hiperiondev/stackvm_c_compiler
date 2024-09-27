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

/**
 * @def TAB_LEN
 * @brief
 *
 */
#define TAB_LEN 4

/**
 * @def it
 * @brief
 *
 */
#define it tab+=TAB_LEN

/**
 * @def dt
 * @brief
 *
 */
#define dt tab-=TAB_LEN

/**
 * @fn char ctype_to_string*(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
char* ctype_to_string(ctype_t *ctype);

/**
 * @fn void uop_to_string(string_t*, char*, ast_t*)
 * @brief
 *
 * @param buf
 * @param op
 * @param ast
 */
void uop_to_string(string_t *buf, char *op, ast_t *ast);

/**
 * @fn void binop_to_string(string_t*, char*, ast_t*)
 * @brief
 *
 * @param buf
 * @param op
 * @param ast
 */
void binop_to_string(string_t *buf, char *op, ast_t *ast);

/**
 * @fn void ast_to_string_int(string_t*, ast_t*, bool)
 * @brief
 *
 * @param buf
 * @param ast
 * @param first_entry
 */
void ast_to_string_int(string_t *buf, ast_t *ast, bool first_entry);

/**
 * @fn char ast_to_string*(ast_t*, bool)
 * @brief
 *
 * @param ast
 * @param first_entry
 * @return
 */
char* ast_to_string(ast_t *ast, bool first_entry);

/**
 * @fn char token_to_string*(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
char* token_to_string(const token_t tok);

#endif /* VERBOSE_H_ */
