/*
 * @util.h
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

#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef struct {
    char *body;
    int nalloc, len;
} String;

static List *cstrings = &EMPTY_LIST;

#define INIT_SIZE 8

static inline String make_string(void) {
    String ret = {
            .body = calloc(1, INIT_SIZE),
            .nalloc = INIT_SIZE,
            .len = 0,
    };
    return ret;
}

static inline void realloc_body(String *s) {
    int newsize = s->nalloc * 2;
    char *body = realloc(s->body, newsize);
    s->body = body;
    s->nalloc = newsize;
}

static inline char* get_cstring(const String s) {
    char *r = s.body;
    list_push(cstrings, r);
    return r;
}

static inline void string_append(String *s, char c) {
    if (s->nalloc == (s->len + 1))
        realloc_body(s);
    s->body[s->len++] = c;
    s->body[s->len] = '\0';
}

static inline void string_appendf(String *s, char *fmt, ...) {
    va_list args;
    while (1) {
        int avail = s->nalloc - s->len;
        va_start(args, fmt);
        int written = vsnprintf(s->body + s->len, avail, fmt, args);
        va_end(args);
        if (avail <= written) {
            realloc_body(s);
            continue;
        }
        s->len += written;
        return;
    }
}

#define error(...) errorf(__FILE__, __LINE__, __VA_ARGS__)

#define assert(expr)                           \
    do {                                       \
        if (!(expr))                           \
            error("Assertion failed: " #expr); \
    } while (0)

static inline void errorf(char *file, int line, char *fmt, ...) {
    fprintf(stderr, "%s:%d: ", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

static inline char* quote_cstring(char *p) {
    String s = make_string();
    for (; *p; p++) {
        if (*p == '\"' || *p == '\\')
            string_appendf(&s, "\\%c", *p);
        else if (*p == '\n')
            string_appendf(&s, "\\n");
        else
            string_append(&s, *p);
    }
    return get_cstring(s);
}

#endif /* UTIL_H */
