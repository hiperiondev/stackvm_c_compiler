/*
 * @list.h
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

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>

#define EMPTY_LIST ((List){.len = 0, .head = NULL, .tail = NULL})
#define list_safe_next(node) ((node) ? (node)->next : NULL)
#define list_for_each_safe(node, tmp, list)                           \
    for ((node) = (list)->head, (tmp) = list_safe_next(node); (node); \
         (node) = (tmp), (tmp) = list_safe_next(node))

typedef struct __ListNode {
    void *elem;
    struct __ListNode *next, *prev;
} ListNode;

typedef struct {
    int len;
    ListNode *head, *tail;
} List;

typedef struct {
    ListNode *ptr;
} Iter;

List* make_list(void);
void* make_node(void *elem);
 void list_push(List *list, void *elem);
void* list_pop(List *list);
 void list_unshift(List *list, void *elem);
 Iter list_iter(void *ptr);
 bool iter_end(const Iter iter);
void* iter_next(Iter *iter);
List* list_reverse(List *list);
  int list_len(List *list);
 void list_free(List *list);

#endif /* LIST_H_ */
