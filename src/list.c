/*
 * @list.c
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
#include <stdlib.h>

#include "util.h"
#include "list.h"

extern void **mkstr;
extern long mkstr_qty;

list_t* list_make(void) {
    list_t *r = malloc(sizeof(list_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->len = 0;
    r->head = r->tail = NULL;
    return r;
}

void* list_make_node(void *elem) {
    list_node_t *r = malloc(sizeof(list_node_t));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->elem = elem;
    r->next = NULL;
    r->prev = NULL;
    return r;
}

void list_push(list_t *list, void *elem) {
    list_node_t *node = list_make_node(elem);
    if (!list->head) {
        list->head = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
    }
    list->tail = node;
    list->len++;
}

void* list_pop(list_t *list) {
    if (!list->head)
        return NULL;
    list_node_t *tail = list->tail;
    void *r = tail->elem;
    list->tail = tail->prev;
    if (list->tail)
        list->tail->next = NULL;
    else
        list->head = NULL;

    for (long n = 0; n < mkstr_qty; n++) {
        if (mkstr[n] == tail)
            mkstr[n] = NULL;
    }

    free(tail);
    tail = NULL;
    return r;
}

void list_unshift(list_t *list, void *elem) {
    list_node_t *node = list_make_node(elem);
    node->next = list->head;
    if (list->head)
        list->head->prev = node;
    list->head = node;
    if (!list->tail)
        list->tail = node;
    list->len++;
}

iter_t list_iter(void *ptr) {
    iter_t ret = { .ptr = ((list_t*) ptr)->head, };
    return ret;
}

bool list_iter_end(const iter_t iter) {
    return !iter.ptr;
}

void* list_iter_next(iter_t *iter) {
    if (!iter->ptr)
        return NULL;
    void *r = iter->ptr->elem;
    iter->ptr = iter->ptr->next;
    return r;
}

list_t* list_reverse(list_t *list) {
    list_t *r = list_make();
    for (iter_t i = list_iter(list); !list_iter_end(i);)
        list_unshift(r, list_iter_next(&i));
    return r;
}

int list_len(list_t *list) {
    return list->len;
}

void list_free(list_t *list) {
    list_node_t *node, *tmp;
    list_for_each_safe (node, tmp, list)
    {
        for (long n = 0; n < mkstr_qty; n++) {
            if (mkstr[n] == node->elem)
                mkstr[n] = NULL;
            if (mkstr[n] == node)
                mkstr[n] = NULL;
        }

        free(node->elem);
        free(node);
    }
}
