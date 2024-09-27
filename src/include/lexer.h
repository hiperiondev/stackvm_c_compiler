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

#define make_null(x)    make_token(TTYPE_NULL,   (uintptr_t) 0)
#define make_strtok(x)  make_token(TTYPE_STRING, (uintptr_t) get_cstring(x))
#define make_ident(x)   make_token(TTYPE_IDENT,  (uintptr_t) get_cstring(x))
#define make_punct(x)   make_token(TTYPE_PUNCT,  (uintptr_t)(x))
#define make_number(x)  make_token(TTYPE_NUMBER, (uintptr_t)(x))
#define make_char(x)    make_token(TTYPE_CHAR,   (uintptr_t)(x))

token_t make_token(enum token_type type, uintptr_t data);
    int getc_nonspace(void);
token_t read_number(char c);
token_t read_char(void);
token_t read_string(void);
token_t read_ident(char c);
   void skip_line_comment(void);
   void skip_block_comment(void);
token_t read_rep(int expect, int t1, int t2);
token_t read_token_int(void);
   bool is_punct(const token_t tok, int c);
   void unget_token(const token_t tok);
token_t peek_token(void);
token_t read_token(void);

#endif /* LEXER_H_ */
