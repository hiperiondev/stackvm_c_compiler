/*
 * @lexer.c
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_stackvm.h"
#include "lexer.h"

static bool ungotten = false;
static token_t ungotten_buf = { 0 };

extern void **mkstr;
extern long mkstr_qty;

token_t make_token(enum token_type type, uintptr_t data) {
    token_t ret = {
            .type = type,
            .priv = data,
    };
    return ret;
}

int getc_nonspace(void) {
    int c;
    while ((c = getc(stdin)) != EOF) {
        if (isspace(c) || c == '\n' || c == '\r')
            continue;
        return c;
    }
    return EOF;
}

token_t read_number(char c) {
    string_t s = make_string();
    add_str_ptr(mkstr, mkstr_qty, s.body);
    string_append(&s, c);
    while (1) {
        int c = getc(stdin);
        if (!isdigit(c) && !isalpha(c) && c != '.') {
            ungetc(c, stdin);
            return make_number(get_cstring(s));
        }
        string_append(&s, c);
    }
}

token_t read_char(void) {
    char c = getc(stdin);
    if (c == EOF)
        goto err;
    if (c == '\\') {
        c = getc(stdin);
        if (c == EOF)
            goto err;
    }
    char c2 = getc(stdin);
    if (c2 == EOF)
        goto err;
    if (c2 != '\'')
        error("Malformed char literal");
    return make_char(c);
err:
    error("Unterminated char");
    return make_null(); /* non-reachable */
}

token_t read_string(void) {
    string_t s = make_string();
    add_str_ptr(mkstr, mkstr_qty, s.body);
    while (1) {
        int c = getc(stdin);
        if (c == EOF)
            error("Unterminated string");
        if (c == '"')
            break;
        if (c == '\\') {
            c = getc(stdin);
            switch (c) {
                case EOF:
                    error("Unterminated \\");
                case '\"':
                    break;
                case 'n':
                    c = '\n';
                    break;
                default:
                    error("Unknown quote: %c", c);
            }
        }
        string_append(&s, c);
    }
    return make_strtok(s);
}

token_t read_ident(char c) {
    string_t s = make_string();
    add_str_ptr(mkstr, mkstr_qty, s.body);
    string_append(&s, c);
    while (1) {
        int c2 = getc(stdin);
        if (isalnum(c2) || c2 == '_') {
            string_append(&s, c2);
        } else {
            ungetc(c2, stdin);
            return make_ident(s);
        }
    }
}

void skip_line_comment(void) {
    while (1) {
        int c = getc(stdin);
        if (c == '\n' || c == EOF)
            return;
    }
}

void skip_block_comment(void) {
    enum {
        in_comment, asterisk_read
    } state = in_comment;
    while (1) {
        int c = getc(stdin);
        if (state == in_comment) {
            if (c == '*')
                state = asterisk_read;
        } else if (c == '/') {
            return;
        }
    }
}

token_t read_rep(int expect, int t1, int t2) {
    int c = getc(stdin);
    if (c == expect)
        return make_punct(t2);
    ungetc(c, stdin);
    return make_punct(t1);
}

token_t read_token_int(void) {
    int c = getc_nonspace();
    switch (c) {
        case '0' ... '9':
            return read_number(c);
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_':
            return read_ident(c);
        case '/': {
            c = getc(stdin);
            if (c == '/') {
                skip_line_comment();
                return read_token_int();
            }
            if (c == '*') {
                skip_block_comment();
                return read_token_int();
            }
            ungetc(c, stdin);
            return make_punct('/');
        }
        case '*':
        case '(':
        case ')':
        case ',':
        case ';':
        case '.':
        case '[':
        case ']':
        case '{':
        case '}':
        case '!':
        case '?':
        case ':':
            return make_punct(c);
        case '-':
            c = getc(stdin);
            if (c == '-')
                return make_punct(PUNCT_DEC);
            if (c == '>')
                return make_punct(PUNCT_ARROW);
            ungetc(c, stdin);
            return make_punct('-');
        case '=':
            return read_rep('=', '=', PUNCT_EQ);
        case '+':
            return read_rep('+', '+', PUNCT_INC);
        case '&':
            return read_rep('&', '&', PUNCT_LOGAND);
        case '|':
            return read_rep('|', '|', PUNCT_LOGOR);
        case '<':
            return read_rep('<', '<', PUNCT_LSHIFT);
        case '>':
            return read_rep('>', '>', PUNCT_RSHIFT);
        case '"':
            return read_string();
        case '\'':
            return read_char();
        case EOF:
            return make_null();
        default:
            error("Unexpected character: '%c'", c);
            return make_null(); /* non-reachable */
    }
}

bool is_punct(const token_t tok, int c) {
    return (get_ttype(tok) == TTYPE_PUNCT) && (get_punct(tok) == c);
}

void unget_token(const token_t tok) {
    if (get_ttype(tok) == TTYPE_NULL)
        return;
    if (ungotten)
        error("Push back buffer is already full");
    ungotten = true;
    ungotten_buf = make_token(tok.type, tok.priv);
}

token_t peek_token(void) {
    token_t tok = read_token();
    unget_token(tok);
    return tok;
}

token_t read_token(void) {
    if (ungotten) {
        ungotten = false;
        return make_token(ungotten_buf.type, ungotten_buf.priv);
    }
    return read_token_int();
}
