/*
 * @main.c
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "c_stackvm.h"
#include "codegenir.h"
#include "parser.h"
#include "verbose.h"
#include "preprocess.h"

FILE *outfp, *preprfp;

static char *outfile = NULL, *infile = NULL;
static bool dump_ast;
extern void **mkstr;

static void usage(void) {
    fprintf(stdout, "stackvm_c_compiler [options] filename\n"
            "OPTIONS\n"
            "  -o filename    Write output to the specified file.\n"
            "  --dump-ast     Dump abstract syntax tree(AST)\n");
}

static void print_usage_and_exit(void) {
    usage();
    exit(1);
}

static void parse_args(int argc, char **argv) {
    if (argc < 2) {
        print_usage_and_exit();
    }

    while (true) {
        argc--;
        argv++;
        if (!argc) {
            break;
        }

        if ((*argv)[0] == '-') {
            switch ((*argv)[1]) {
                case '\0':
                    infile = "/dev/stdin";
                    break;
                case 'o':
                    argc--;
                    argv++;
                    outfile = *argv;
                    break;
                case '-':
                    if (!strcmp(*argv, "--dump-ast"))
                        dump_ast = true;
                    break;
                default:
                    print_usage_and_exit();
            }
        } else {
            if (infile)
                print_usage_and_exit();

            infile = argv[0];
        }
    }
}

static void open_output_file(void) {
    if (outfile) {
        if (!(outfp = fopen(outfile, "w"))) {
            printf("Can't open file %s\n", outfile);
            exit(1);
        }
    } else {
        outfp = stdout;
    }
}

static void open_input_file(void) {
    if (!infile) {
        printf("Input file is not specified\n\n");
        print_usage_and_exit();
    }

    if (!(preprfp = fopen(infile, "r"))) {
        printf("Can't open file %s\n", outfile);
        exit(1);
    }

    if (!freopen(infile, "rw", stdin)) {
        printf("Can't open file %s\n", infile);
        exit(1);
    }

    preprocess_file(preprfp, stdin);

    fclose(preprfp);
}

int main(int argc, char **argv) {
    parse_args(argc, argv);
    open_input_file();
    open_output_file();
    mkstr = malloc(sizeof(void*));

    list_t *toplevels = parser_read_toplevels();
    if (!dump_ast)
        codegenir_emit_data_section();

    for (iter_t i = list_iter(toplevels); !list_iter_end(i);) {
        ast_t *v = list_iter_next(&i);
        if (dump_ast) {
            char *aststr = verbose_ast_to_string(v, true);
            printf("%s \n", aststr);
            util_lfree(aststr);
        } else {
            codegenir_emit_toplevel(v);
        }
    }

    printf("\n---- data section ----\n");
    codegenir_emit_data_section();
    printf("\n----------------------\n");

    util_free_all();

    free(mkstr);
    fclose(outfp);

    return 0;
}
