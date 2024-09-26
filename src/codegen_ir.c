/*
 * @codegenir.c
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

#include "c_stackvm.h"

static int TAB = 8;
FILE *outfp;
static List *functions = &EMPTY_LIST;

#define emit(...) emitf(__LINE__, "\t" __VA_ARGS__)
#define emit_label(...) emitf(__LINE__, __VA_ARGS__)
#define UNUSED __attribute__((unused))
#define SAVE()                                                   \
    int save_hook __attribute__((cleanup(pop_function))) UNUSED; \
    list_push(functions, (void *) __func__)

static void pop_function(void *ignore UNUSED) {
    list_pop(functions);
}

static char* get_caller_list(void) {
    String s = make_string();
    for (Iter i = list_iter(functions); !iter_end(i);) {
        string_appendf(&s, "%s", iter_next(&i));
        if (!iter_end(i))
            string_appendf(&s, " -> ");
    }
    return get_cstring(s);
}

static void emitf(int line, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int col = vfprintf(outfp, fmt, args);
    va_end(args);

    for (char *p = fmt; *p; p++)
        if (*p == '\t')
            col += TAB - 1;
    int space = (28 - col) > 0 ? (30 - col) : 2;
    fprintf(outfp, "%*c %s:%d\n", space, '#', get_caller_list(), line);
}

void emit_data_section(void) {
    SAVE();
    emit(".data");
    for (Iter i = list_iter(strings); !iter_end(i);) {
        Ast *v = iter_next(&i);
        emit_label("%s:", v->slabel);
        char *cstr = quote_cstring(v->sval);
        emit(".string \"%s\"", cstr);
        lfree(cstr);
    }
    for (Iter i = list_iter(flonums); !iter_end(i);) {
        Ast *v = iter_next(&i);
        char *label = make_label();
        v->flabel = label;
        emit_label("%s:", label);
        emit(".long %d", v->lval[0]);
        emit(".long %d", v->lval[1]);
    }
}

////////////////////////////////////////////////////////////////////////

void emit_toplevel(Ast *v) {

}
