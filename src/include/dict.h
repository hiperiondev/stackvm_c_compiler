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

typedef struct dict_s {
           list_t *list;
    struct dict_s *parent;
} dict_t;

#define EMPTY_DICT ((dict_t){&EMPTY_LIST, NULL})

typedef struct {
    char *key;
    void *val;
} dict_entry_t;

  void* make_dict(void *parent);
  void* dict_get(dict_t *dict, char *key);
   void dict_put(dict_t *dict, char *key, void *val);
list_t* dict_keys(dict_t *dict);
list_t* dict_values(dict_t *dict);
  void* dict_parent(dict_t *dict);

#endif /* DICT_H_ */
