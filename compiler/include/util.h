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

/**
 * @def INIT_SIZE
 * @brief
 *
 */
#define INIT_SIZE 8

/**
 * @def add_str_ptr
 * @brief
 *
 */
#define add_str_ptr(list, qty, ptr)                        \
            list[qty++] = (void*)(ptr);                    \
            list = realloc(list, (qty + 1) * sizeof(void*))

/**
 * @def error
 * @brief
 *
 */
#define error(...) util_errorf(__FILE__, __LINE__, __VA_ARGS__)

/**
 * @def assert
 * @brief
 *
 */
#define assert(expr)                           \
    do {                                       \
        if (!(expr))                           \
            error("Assertion failed: " #expr); \
    } while (0)

/**
 * @def swap
 * @brief
 *
 */
#define swap(a, b)         \
    {                      \
        typeof(a) tmp = b; \
        b = a;             \
        a = tmp;           \
    }

/**
 * @struct
 * @brief
 *
 */
typedef struct {
    char *body;
      int nalloc, len;
} string_t;

/**
 * @fn void util_free_all(void)
 * @brief
 *
 */
void util_free_all(void);

/**
 * @fn void util_lfree(void*)
 * @brief
 *
 * @param ptr
 */
void util_lfree(void *ptr);

/**
 * @fn string_t util_make_string(void)
 * @brief
 *
 * @return
 */
string_t util_make_string(void);

/**
 * @fn void util_realloc_body(string_t*)
 * @brief
 *
 * @param s
 */
void util_realloc_body(string_t *s);

/**
 * @fn char util_get_cstring*(const string_t)
 * @brief
 *
 * @param s
 * @return
 */
char* util_get_cstring(const string_t s);

/**
 * @fn void util_string_append(string_t*, char)
 * @brief
 *
 * @param s
 * @param c
 */
void util_string_append(string_t *s, char c);

/**
 * @fn void util_string_appendf(string_t*, char*, ...)
 * @brief
 *
 * @param s
 * @param fmt
 */
void util_string_appendf(string_t *s, char *fmt, ...);

/**
 * @fn void util_errorf(char*, int, char*, ...)
 * @brief
 *
 * @param file
 * @param line
 * @param fmt
 */
void util_errorf(char *file, int line, char *fmt, ...);

/**
 * @fn char util_quote_cstring*(char*)
 * @brief
 *
 * @param p
 * @return
 */
char* util_quote_cstring(char *p);

#endif /* UTIL_H */
