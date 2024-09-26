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
#include <limits.h>

#include "list.h"

extern void **mkstr;
extern long mkstr_qty;

#define add_str_ptr(list, qty, ptr)                        \
            list[qty++] = (void*)(ptr);                    \
            list = realloc(list, (qty + 1) * sizeof(void*))

typedef struct {
    char *body;
    int nalloc, len;
} String;

static List *cstrings = &EMPTY_LIST;

#define INIT_SIZE 8

inline void lfree(void *ptr) {
    long pos = LONG_MAX;

    for (long n = 0; n < mkstr_qty; n++)
        if ((void*) mkstr[n] == ptr) {
            pos = n;
            break;
        }

    if (pos != LONG_MAX) {
        mkstr[pos] = NULL;
    }

    free(ptr);
}

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

    add_str_ptr(mkstr, mkstr_qty, s->body);

}

static inline char* get_cstring(const String s) {
    char *r = s.body;
    list_push(cstrings, r);
    return r;
}

static inline void string_append(String *s, char c) {
    long pos = LONG_MAX;

    if (s->nalloc == (s->len + 1)) {
        for (long n = 0; n < mkstr_qty; n++)
            if ((void*) mkstr[n] == (void*) s->body) {
                pos = n;
                break;
            }

        if (pos != LONG_MAX)
            mkstr[pos] = NULL;

        realloc_body(s);
    }
    s->body[s->len++] = c;
    s->body[s->len] = '\0';
}

static inline void string_appendf(String *s, char *fmt, ...) {
    va_list args;
    long pos = LONG_MAX;
    while (1) {
        int avail = s->nalloc - s->len;
        va_start(args, fmt);
        int written = vsnprintf(s->body + s->len, avail, fmt, args);
        va_end(args);
        if (avail <= written) {
            for (long n = 0; n < mkstr_qty; n++)
                if ((void*) mkstr[n] == (void*) s->body) {
                    pos = n;
                    break;
                }

            if (pos != LONG_MAX)
                mkstr[pos] = NULL;

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
