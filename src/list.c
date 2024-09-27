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

List* make_list(void) {
    List *r = malloc(sizeof(List));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->len = 0;
    r->head = r->tail = NULL;
    return r;
}

void* make_node(void *elem) {
    ListNode *r = malloc(sizeof(ListNode));
    add_str_ptr(mkstr, mkstr_qty, r);
    r->elem = elem;
    r->next = NULL;
    r->prev = NULL;
    return r;
}

void list_push(List *list, void *elem) {
    ListNode *node = make_node(elem);
    if (!list->head) {
        list->head = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
    }
    list->tail = node;
    list->len++;
}

void* list_pop(List *list) {
    if (!list->head)
        return NULL;
    ListNode *tail = list->tail;
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

void list_unshift(List *list, void *elem) {
    ListNode *node = make_node(elem);
    node->next = list->head;
    if (list->head)
        list->head->prev = node;
    list->head = node;
    if (!list->tail)
        list->tail = node;
    list->len++;
}

Iter list_iter(void *ptr) {
    Iter ret = { .ptr = ((List*) ptr)->head, };
    return ret;
}

bool iter_end(const Iter iter) {
    return !iter.ptr;
}

void* iter_next(Iter *iter) {
    if (!iter->ptr)
        return NULL;
    void *r = iter->ptr->elem;
    iter->ptr = iter->ptr->next;
    return r;
}

List* list_reverse(List *list) {
    List *r = make_list();
    for (Iter i = list_iter(list); !iter_end(i);)
        list_unshift(r, iter_next(&i));
    return r;
}

int list_len(List *list) {
    return list->len;
}

void list_free(List *list) {
    ListNode *node, *tmp;
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
