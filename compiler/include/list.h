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

/**
 * @def list_empty
 * @brief
 *
 */
#define list_empty ((list_t){.len = 0, .head = NULL, .tail = NULL})

/**
 * @def list_safe_next
 * @brief
 *
 */
#define list_safe_next(node) ((node) ? (node)->next : NULL)

/**
 * @def list_for_each_safe
 * @brief
 *
 */
#define list_for_each_safe(node, tmp, list)                           \
    for ((node) = (list)->head, (tmp) = list_safe_next(node); (node); \
         (node) = (tmp), (tmp) = list_safe_next(node))

/**
 * @struct list_node_s
 * @brief
 *
 */
typedef struct list_node_s {
                  void *elem;
    struct list_node_s *next, *prev;
} list_node_t;

/**
 * @struct
 * @brief
 *
 */
typedef struct {
            int len;
    list_node_t *head, *tail;
} list_t;

/**
 * @struct
 * @brief
 *
 */
typedef struct {
    list_node_t *ptr;
} iter_t;

/**
 * @fn list_t list_make*(void)
 * @brief
 *
 * @return
 */
list_t* list_make(void);

/**
 * @fn void list_make_node*(void*)
 * @brief
 *
 * @param elem
 */
void* list_make_node(void *elem);

/**
 * @fn void list_push(list_t*, void*)
 * @brief
 *
 * @param list
 * @param elem
 */
void list_push(list_t *list, void *elem);

/**
 * @fn void list_pop*(list_t*)
 * @brief
 *
 * @param list
 */
void* list_pop(list_t *list);

/**
 * @fn void list_unshift(list_t*, void*)
 * @brief
 *
 * @param list
 * @param elem
 */
void list_unshift(list_t *list, void *elem);

/**
 * @fn iter_t list_iter(void*)
 * @brief
 *
 * @param ptr
 * @return
 */
iter_t list_iter(void *ptr);

/**
 * @fn bool list_iter_end(const iter_t)
 * @brief
 *
 * @param iter
 * @return
 */
bool list_iter_end(const iter_t iter);

/**
 * @fn void list_iter_next*(iter_t*)
 * @brief
 *
 * @param iter
 */
void* list_iter_next(iter_t *iter);

/**
 * @fn list_t list_reverse*(list_t*)
 * @brief
 *
 * @param list
 * @return
 */
list_t* list_reverse(list_t *list);

/**
 * @fn int list_len(list_t*)
 * @brief
 *
 * @param list
 * @return
 */
int list_len(list_t *list);

/**
 * @fn void list_free(list_t*)
 * @brief
 *
 * @param list
 */
void list_free(list_t *list);

#endif /* LIST_H_ */
