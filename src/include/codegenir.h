/*
 * @codegen_ir.h
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

#ifndef CODEGEN_IR_H_
#define CODEGEN_IR_H_

#define emit(...) codegenir_emitf(__LINE__, "\t" __VA_ARGS__)
#define emit_label(...) codegenir_emitf(__LINE__, __VA_ARGS__)
#define UNUSED __attribute__((unused))
#define SAVE()                                                             \
    int save_hook __attribute__((cleanup(codegenir_pop_function))) UNUSED; \
    list_push(functions, (void *) __func__)

 void codegenir_pop_function(void *ignore UNUSED);
char* codegenir_get_caller_list(void);
 void codegenir_emitf(int line, char *fmt, ...);
 void codegenir_emit_data_section(void);
 void codegenir_emit_toplevel(ast_t *v);

#endif /* CODEGEN_IR_H_ */
