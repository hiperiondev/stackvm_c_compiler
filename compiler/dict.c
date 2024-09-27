/*
 * @dict.c
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

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "list.h"
#include "dict.h"

extern void **mkstr;
extern long mkstr_qty;

void* dict_make(void *parent) {
    dict_t *r = malloc(sizeof(dict_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->list = list_make();
    r->parent = parent;
    return r;
}

void* dict_get(dict_t *dict, char *key) {
    for (; dict; dict = dict->parent) {
        for (iter_t i = list_iter(dict->list); !list_iter_end(i);) {
            dict_entry_t *e = list_iter_next(&i);
            if (!strcmp(key, e->key))
                return e->val;
        }
    }
    return NULL;
}

void dict_put(dict_t *dict, char *key, void *val) {
    dict_entry_t *e = malloc(sizeof(dict_entry_t));
    add_str_ptr(mkstr, mkstr_qty, e);
    e->key = key;
    e->val = val;
    list_push(dict->list, e);
}

list_t* dict_keys(dict_t *dict) {
    list_t *r = list_make();
    for (; dict; dict = dict->parent)
        for (iter_t i = list_iter(dict->list); !list_iter_end(i);)
            list_push(r, ((dict_entry_t*) list_iter_next(&i))->key);
    return r;
}

list_t* dict_values(dict_t *dict) {
    list_t *r = list_make();
    for (; dict; dict = dict->parent)
        for (iter_t i = list_iter(dict->list); !list_iter_end(i);)
            list_push(r, ((dict_entry_t*) list_iter_next(&i))->val);
    return r;
}

void* dict_parent(dict_t *dict) {
    void *r = dict->parent;

    for (long n = 0; n < mkstr_qty; n++) {
        if (mkstr[n] == dict->list)
            mkstr[n] = NULL;
        if (mkstr[n] == dict)
            mkstr[n] = NULL;
    }

    list_free(dict->list);
    free(dict->list);
    free(dict);
    return r;
}
