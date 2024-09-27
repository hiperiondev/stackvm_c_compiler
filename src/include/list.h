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

#define EMPTY_LIST ((list_t){.len = 0, .head = NULL, .tail = NULL})
#define list_safe_next(node) ((node) ? (node)->next : NULL)
#define list_for_each_safe(node, tmp, list)                           \
    for ((node) = (list)->head, (tmp) = list_safe_next(node); (node); \
         (node) = (tmp), (tmp) = list_safe_next(node))

typedef struct list_node_s {
                  void *elem;
    struct list_node_s *next, *prev;
} list_node_t;

typedef struct {
            int len;
    list_node_t *head, *tail;
} list_t;

typedef struct {
    list_node_t *ptr;
} iter_t;

list_t* make_list(void);
  void* make_node(void *elem);
   void list_push(list_t *list, void *elem);
  void* list_pop(list_t *list);
   void list_unshift(list_t *list, void *elem);
 iter_t list_iter(void *ptr);
   bool iter_end(const iter_t iter);
  void* iter_next(iter_t *iter);
list_t* list_reverse(list_t *list);
    int list_len(list_t *list);
   void list_free(list_t *list);

#endif /* LIST_H_ */
