/*
 * @lexer.h
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

#ifndef LEXER_H_
#define LEXER_H_

#include <stdbool.h>

/**
 * @fn token_t lexer_make_token(enum token_type, uintptr_t)
 * @brief
 *
 * @param type
 * @param data
 * @return
 */
token_t lexer_make_token(enum token_type type, uintptr_t data);

/**
 * @fn int lexer_getc_nonspace(void)
 * @brief
 *
 * @return
 */
int lexer_getc_nonspace(void);

/**
 * @fn token_t lexer_read_number(char)
 * @brief
 *
 * @param c
 * @return
 */
token_t lexer_read_number(char c);

/**
 * @fn token_t lexer_read_char(void)
 * @brief
 *
 * @return
 */
token_t lexer_read_char(void);

/**
 * @fn token_t lexer_read_string(void)
 * @brief
 *
 * @return
 */
token_t lexer_read_string(void);

/**
 * @fn token_t lexer_read_ident(char)
 * @brief
 *
 * @param c
 * @return
 */
token_t lexer_read_ident(char c);

/**
 * @fn void lexer_skip_line_comment(void)
 * @brief
 *
 */
void lexer_skip_line_comment(void);

/**
 * @fn void lexer_skip_block_comment(void)
 * @brief
 *
 */
void lexer_skip_block_comment(void);

/**
 * @fn token_t lexer_read_rep(int, int, int)
 * @brief
 *
 * @param expect
 * @param t1
 * @param t2
 * @return
 */
token_t lexer_read_rep(int expect, int t1, int t2);

/**
 * @fn token_t lexer_read_token_int(void)
 * @brief
 *
 * @return
 */
token_t lexer_read_token_int(void);

/**
 * @fn bool lexer_is_punct(const token_t, int)
 * @brief
 *
 * @param tok
 * @param c
 * @return
 */
bool lexer_is_punct(const token_t tok, int c);

/**
 * @fn void lexer_unget_token(const token_t)
 * @brief
 *
 * @param tok
 */
void lexer_unget_token(const token_t tok);

/**
 * @fn token_t lexer_peek_token(void)
 * @brief
 *
 * @return
 */
token_t lexer_peek_token(void);

/**
 * @fn token_t lexer_read_token(void)
 * @brief
 *
 * @return
 */
token_t lexer_read_token(void);

#endif /* LEXER_H_ */
