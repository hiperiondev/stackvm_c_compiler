/*
 * @dict.h
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

#ifndef DICT_H_
#define DICT_H_

#include "list.h"

typedef struct Dict {
           List *list;
    struct Dict *parent;
} Dict;

#define EMPTY_DICT ((Dict){&EMPTY_LIST, NULL})

typedef struct {
    char *key;
    void *val;
} DictEntry;

void* make_dict(void *parent);
void* dict_get(Dict *dict, char *key);
 void dict_put(Dict *dict, char *key, void *val);
List* dict_keys(Dict *dict);
List* dict_values(Dict *dict);
void* dict_parent(Dict *dict);

#endif /* DICT_H_ */
