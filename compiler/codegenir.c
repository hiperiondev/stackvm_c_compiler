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
#include "codegenir.h"

FILE *outfp;
static list_t *functions = &list_empty;
extern list_t *strings;
extern list_t *flonums;

char* codegenir_get_caller_list(void) {
    string_t s = util_make_string();
    for (iter_t i = list_iter(functions); !list_iter_end(i);) {
        util_string_appendf(&s, "%s", list_iter_next(&i));
        if (!list_iter_end(i))
            util_string_appendf(&s, " -> ");
    }
    return util_get_cstring(s);
}

void codegenir_emitf(int line, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int col = vfprintf(outfp, fmt, args);
    va_end(args);

    for (char *p = fmt; *p; p++)
        if (*p == '\t')
            col += TAB_LEN - 1;

    fprintf(outfp, "\n");
}

void codegenir_emit_data_section(void) {
    //SAVE();
    codegenir_emit(".data");
    for (iter_t i = list_iter(strings); !list_iter_end(i);) {
        ast_t *v = list_iter_next(&i);
        codegenir_emit_label("%s:", v->slabel);
        char *cstr = util_quote_cstring(v->sval);
        codegenir_emit(".string \"%s\"", cstr);
        util_lfree(cstr);
    }
    for (iter_t i = list_iter(flonums); !list_iter_end(i);) {
        ast_t *v = list_iter_next(&i);
        char *label = parser_make_label();
        v->flabel = label;
        codegenir_emit_label("%s:", label);
        codegenir_emit(".long %d", v->lval[0]);
        codegenir_emit(".long %d", v->lval[1]);
    }
}

////////////////////////////////////////////////////////////////////////

void codegenir_emit_toplevel(ast_t *v) {

}
