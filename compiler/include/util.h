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

#define INIT_SIZE 8

#define add_str_ptr(list, qty, ptr)                        \
            list[qty++] = (void*)(ptr);                    \
            list = realloc(list, (qty + 1) * sizeof(void*))

#define error(...) util_errorf(__FILE__, __LINE__, __VA_ARGS__)

#define assert(expr)                           \
    do {                                       \
        if (!(expr))                           \
            error("Assertion failed: " #expr); \
    } while (0)

#define swap(a, b)         \
    {                      \
        typeof(a) tmp = b; \
        b = a;             \
        a = tmp;           \
    }

typedef struct {
    char *body;
      int nalloc, len;
} string_t;

    void util_free_all(void);
    void util_lfree(void *ptr);
string_t util_make_string(void);
    void util_realloc_body(string_t *s);
   char* util_get_cstring(const string_t s);
    void util_string_append(string_t *s, char c);
    void util_string_appendf(string_t *s, char *fmt, ...);
    void util_errorf(char *file, int line, char *fmt, ...);
   char* util_quote_cstring(char *p);

#endif /* UTIL_H */
