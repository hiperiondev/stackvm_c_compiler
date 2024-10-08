/*
 * @util.c
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "util.h"
#include "list.h"

void **mkstr = NULL;
long mkstr_qty = 0;

static list_t *cstrings = &list_empty;

void util_free_all(void) {
    for (long n = 0; n < mkstr_qty; n++) {
        if (mkstr[n] != NULL)
            free(mkstr[n]);
        mkstr[n] = NULL;
    }
}

void util_lfree(void *ptr) {
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

string_t util_make_string(void) {
    string_t ret = { .body = calloc(1, INIT_SIZE), .nalloc = INIT_SIZE, .len = 0, };

    return ret;
}

void util_realloc_body(string_t *s) {
    int newsize = s->nalloc * 2;

    char *body = realloc(s->body, newsize);
    s->body = body;
    s->nalloc = newsize;

    add_str_ptr(mkstr, mkstr_qty, s->body);

}

char* util_get_cstring(const string_t s) {
    char *r = s.body;
    list_push(cstrings, r);
    return r;
}

void util_string_append(string_t *s, char c) {
    long pos = LONG_MAX;

    if (s->nalloc == (s->len + 1)) {
        for (long n = 0; n < mkstr_qty; n++)
            if ((void*) mkstr[n] == (void*) s->body) {
                pos = n;
                break;
            }

        if (pos != LONG_MAX)
            mkstr[pos] = NULL;

        util_realloc_body(s);
    }
    s->body[s->len++] = c;
    s->body[s->len] = '\0';
}

void util_string_appendf(string_t *s, char *fmt, ...) {
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

            util_realloc_body(s);
            continue;
        }
        s->len += written;
        return;
    }
}

void util_errorf(char *file, int line, char *fmt, ...) {
    fprintf(stderr, "%s:%d: ", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    util_free_all();
    exit(1);
}

char* util_quote_cstring(char *p) {
    string_t s = util_make_string();
    for (; *p; p++) {
        if (*p == '\"' || *p == '\\')
            util_string_appendf(&s, "\\%c", *p);
        else if (*p == '\n')
            util_string_appendf(&s, "\\n");
        else
            util_string_append(&s, *p);
    }
    return util_get_cstring(s);
}
