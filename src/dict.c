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

void* make_dict(void *parent) {
    Dict *r = malloc(sizeof(Dict));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->list = make_list();
    r->parent = parent;
    return r;
}

void* dict_get(Dict *dict, char *key) {
    for (; dict; dict = dict->parent) {
        for (Iter i = list_iter(dict->list); !iter_end(i);) {
            DictEntry *e = iter_next(&i);
            if (!strcmp(key, e->key))
                return e->val;
        }
    }
    return NULL;
}

void dict_put(Dict *dict, char *key, void *val) {
    DictEntry *e = malloc(sizeof(DictEntry));
    add_str_ptr(mkstr, mkstr_qty, e);
    e->key = key;
    e->val = val;
    list_push(dict->list, e);
}

List* dict_keys(Dict *dict) {
    List *r = make_list();
    for (; dict; dict = dict->parent)
        for (Iter i = list_iter(dict->list); !iter_end(i);)
            list_push(r, ((DictEntry*) iter_next(&i))->key);
    return r;
}

List* dict_values(Dict *dict) {
    List *r = make_list();
    for (; dict; dict = dict->parent)
        for (Iter i = list_iter(dict->list); !iter_end(i);)
            list_push(r, ((DictEntry*) iter_next(&i))->val);
    return r;
}

void* dict_parent(Dict *dict) {
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
