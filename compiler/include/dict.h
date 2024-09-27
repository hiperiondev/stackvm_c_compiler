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

/**
 * @struct dict_s
 * @brief
 *
 */
typedef struct dict_s {
           list_t *list;
    struct dict_s *parent;
} dict_t;

/**
 * @def list_empty_dict
 * @brief
 *
 */
#define list_empty_dict ((dict_t){&EMPTY_LIST, NULL})

/**
 * @struct
 * @brief
 *
 */
typedef struct {
    char *key;
    void *val;
} dict_entry_t;

/**
 * @fn void dict_make*(void*)
 * @brief
 *
 * @param parent
 */
void* dict_make(void *parent);

/**
 * @fn void dict_get*(dict_t*, char*)
 * @brief
 *
 * @param dict
 * @param key
 */
void* dict_get(dict_t *dict, char *key);

/**
 * @fn void dict_put(dict_t*, char*, void*)
 * @brief
 *
 * @param dict
 * @param key
 * @param val
 */
void dict_put(dict_t *dict, char *key, void *val);

/**
 * @fn list_t dict_keys*(dict_t*)
 * @brief
 *
 * @param dict
 * @return
 */
list_t* dict_keys(dict_t *dict);

/**
 * @fn list_t dict_values*(dict_t*)
 * @brief
 *
 * @param dict
 * @return
 */
list_t* dict_values(dict_t *dict);

/**
 * @fn void dict_parent*(dict_t*)
 * @brief
 *
 * @param dict
 */
void* dict_parent(dict_t *dict);

#endif /* DICT_H_ */
