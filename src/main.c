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

#include "c_stackvm.h"

static char *outfile = NULL, *infile = NULL;
extern FILE *outfp;
static bool dump_ast;

static void usage() {
    fprintf(stdout, "stackvm_c_compiler [options] filename\n"
            "OPTIONS\n"
            "  -o filename            Write output to the specified file.\n"
            "  --dump-ast             Dump abstract syntax tree(AST)\n");
}

static void print_usage_and_exit() {
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
            if (infile) {
                // The second non-option argument is not what we expect.
                print_usage_and_exit();
            }
            infile = argv[0];
        }
    }
}

static void open_output_file() {
    if (outfile) {
        if (!(outfp = fopen(outfile, "w"))) {
            printf("Can't open file %s\n", outfile);
            exit(1);
        }
    } else {
        outfp = stdout;
    }
}

static void open_input_file() {
    if (!infile) {
        printf("Input file is not specified\n\n");
        print_usage_and_exit();
    }

    if (!freopen(infile, "r", stdin)) {
        printf("Can't open file %s\n", infile);
        exit(1);
    }
}

int main(int argc, char **argv) {
    parse_args(argc, argv);
    open_input_file();
    open_output_file();

    List *toplevels = read_toplevels();
    if (!dump_ast)
        emit_data_section();

    for (Iter i = list_iter(toplevels); !iter_end(i);) {
        Ast *v = iter_next(&i);
        if (dump_ast) {
            char *aststr = ast_to_string(v, true);
            printf("%s \n", aststr);
            free(aststr);
        } else {
            emit_toplevel(v);
        }
    }
    list_free(cstrings);
    list_free(ctypes);

    fclose(outfp);

    return 0;
}
