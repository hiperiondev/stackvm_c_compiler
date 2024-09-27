/*
 * @codegen_ir.c
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
#include "parser.h"
#include "util.h"
#include "list.h"
#include "codegen_ir.h"

static int TAB = 8;
FILE *outfp;
static list_t *functions = &EMPTY_LIST;
extern list_t *strings;
extern list_t *flonums;

void pop_function(void *ignore UNUSED) {
    list_pop(functions);
}

char* get_caller_list(void) {
    string_t s = make_string();
    for (iter_t i = list_iter(functions); !iter_end(i);) {
        string_appendf(&s, "%s", iter_next(&i));
        if (!iter_end(i))
            string_appendf(&s, " -> ");
    }
    return get_cstring(s);
}

void emitf(int line, char *fmt, ...) {
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
    for (iter_t i = list_iter(strings); !iter_end(i);) {
        ast_t *v = iter_next(&i);
        emit_label("%s:", v->slabel);
        char *cstr = quote_cstring(v->sval);
        emit(".string \"%s\"", cstr);
        lfree(cstr);
    }
    for (iter_t i = list_iter(flonums); !iter_end(i);) {
        ast_t *v = iter_next(&i);
        char *label = make_label();
        v->flabel = label;
        emit_label("%s:", label);
        emit(".long %d", v->lval[0]);
        emit(".long %d", v->lval[1]);
    }
}

////////////////////////////////////////////////////////////////////////

void emit_toplevel(ast_t *v) {

}
