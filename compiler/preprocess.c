/*
 * @preprocess.c
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

#include <stdio.h>

static void preprocess_switch_to_if(FILE *file_in, FILE *file_out) {

}

static void preprocess_include(FILE *file_in, FILE *file_out) {

}

///////////////////////////////////////////////////////////////////

void preprocess_file(FILE *file_in, FILE *file_out) {
    FILE *tempfp = tmpfile();
    int c;

    preprocess_include(file_in, tempfp);
    preprocess_switch_to_if(file_in, tempfp);

    while ((c = fgetc(tempfp)) != EOF) {
        fputc(c, file_out);
    }

    fclose(tempfp);
}

