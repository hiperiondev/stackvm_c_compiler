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
 * @def it
 * @brief
 *
 */
#define it tab += TAB_LEN

/**
 * @def dt
 * @brief
 *
 */
#define dt tab -= TAB_LEN

/**
 * @fn char verbose_ctype_to_string*(ctype_t*)
 * @brief
 *
 * @param ctype
 * @return
 */
char* verbose_ctype_to_string(ctype_t *ctype);

/**
 * @fn void verbose_uop_to_string(string_t*, char*, ast_t*)
 * @brief
 *
 * @param buf
 * @param op
 * @param ast
 */
void verbose_uop_to_string(string_t *buf, char *op, ast_t *ast);

/**
 * @fn void verbose_binop_to_string(string_t*, char*, ast_t*)
 * @brief
 *
 * @param buf
 * @param op
 * @param ast
 */
void verbose_binop_to_string(string_t *buf, char *op, ast_t *ast);

/**
 * @fn void verbose_ast_to_string_int(string_t*, ast_t*, bool)
 * @brief
 *
 * @param buf
 * @param ast
 * @param first_entry
 */
void verbose_ast_to_string_int(string_t *buf, ast_t *ast, bool first_entry);

/**
 * @fn char verbose_ast_to_string*(ast_t*, bool)
 * @brief
 *
 * @param ast
 * @param first_entry
 * @return
 */
char* verbose_ast_to_string(ast_t *ast, bool first_entry);

/**
 * @fn char verbose_token_to_string*(const token_t)
 * @brief
 *
 * @param tok
 * @return
 */
char* verbose_token_to_string(const token_t tok);

#endif /* VERBOSE_H_ */
