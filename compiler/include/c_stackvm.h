/*
 * @c_stackvm.h
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

#ifndef C_STACKVM_H
#define C_STACKVM_H

#include <stdbool.h>
#include <stdint.h>

//#define ALLOW_DOUBLE

#include "dict.h"
#include "list.h"
#include "util.h"

/**
 * @def TAB_LEN
 * @brief
 *
 */
#define TAB_LEN 4

/**
 * @enum token_type
 * @brief
 *
 */
enum token_type {
    TTYPE_NULL,  /**< TTYPE_NULL */
    TTYPE_IDENT, /**< TTYPE_IDENT */
    TTYPE_PUNCT, /**< TTYPE_PUNCT */
    TTYPE_NUMBER,/**< TTYPE_NUMBER */
    TTYPE_CHAR,  /**< TTYPE_CHAR */
    TTYPE_STRING,/**< TTYPE_STRING */
};

/**
 * @struct
 * @brief
 *
 */
typedef struct {
          int type;
    uintptr_t priv;
} token_t;

/**
 * @enum
 * @brief
 *
 */
enum {
    AST_LITERAL = 256, /**< AST_LITERAL */
    AST_STRING,        /**< AST_STRING */
    AST_LVAR,          /**< AST_LVAR */
    AST_GVAR,          /**< AST_GVAR */
    AST_FUNCALL,       /**< AST_FUNCALL */
    AST_FUNC,          /**< AST_FUNC */
    AST_DECL,          /**< AST_DECL */
    AST_ARRAY_INIT,    /**< AST_ARRAY_INIT */
    AST_ADDR,          /**< AST_ADDR */
    AST_DEREF,         /**< AST_DEREF */
    AST_IF,            /**< AST_IF */
    AST_TERNARY,       /**< AST_TERNARY */
    AST_FOR,           /**< AST_FOR */
    AST_RETURN,        /**< AST_RETURN */
    AST_COMPOUND_STMT, /**< AST_COMPOUND_STMT */
    AST_STRUCT_REF,    /**< AST_STRUCT_REF */
    PUNCT_EQ,          /**< PUNCT_EQ */
    PUNCT_INC,         /**< PUNCT_INC */
    PUNCT_DEC,         /**< PUNCT_DEC */
    PUNCT_LOGAND,      /**< PUNCT_LOGAND */
    PUNCT_LOGOR,       /**< PUNCT_LOGOR */
    PUNCT_ARROW,       /**< PUNCT_ARROW */
    PUNCT_LSHIFT,      /**< PUNCT_LSHIFT */
    PUNCT_RSHIFT,      /**< PUNCT_RSHIFT */
    PUNCT_PREINC,      /**< PUNCT_PREINC */
    PUNCT_PREDEC,      /**< PUNCT_PREDEC */
};

/**
 * @enum
 * @brief
 *
 */
enum {
    CTYPE_VOID,    /**< CTYPE_VOID */
    CTYPE_CHAR,    /**< CTYPE_CHAR */
    CTYPE_INT,     /**< CTYPE_INT */
    CTYPE_LONG,    /**< CTYPE_LONG */
    CTYPE_FLOAT,   /**< CTYPE_FLOAT */
    CTYPE_UINT,    /**< CTYPE_UINT */
#ifdef ALLOW_DOUBLE
    CTYPE_DOUBLE,  /**< CTYPE_DOUBLE */
#endif
    CTYPE_ARRAY,   /**< CTYPE_ARRAY */
    CTYPE_PTR,     /**< CTYPE_PTR */
    CTYPE_STRUCT,  /**< CTYPE_STRUCT */
};

/**
 * @struct ctype_s
 * @brief
 *
 */
typedef struct ctype_s {
                int type;   //
                int size;   //

    struct ctype_s *ptr;    // pointer or array
               int len;     // array length

            dict_t *fields; // struct
               int offset;  // struct
} ctype_t;

/**
 * @struct ast_s
 * @brief
 *
 */
typedef struct ast_s {
    int type;
    ctype_t *ctype;
    union {
        // char, int, or long
        long ival;

        // float or double
        struct {
            union {
                double fval;
                   int lval[2];
            };
            char *flabel;
        };

        // string literal
        struct {
            char *sval;
            char *slabel;
        };

        // Local/global variable
        struct {
            char *varname;
            struct {
                 int loff;
                char *glabel;
            };
        };

        // Binary operator
        struct {
            struct ast_s *left;
            struct ast_s *right;
        };

        // Unary operator */
        struct {
            struct ast_s *operand;
        };

        // Function call or function declaration
        struct {
            char *fname;
            struct {
                list_t *args;
                struct {
                          list_t *params;
                          list_t *localvars;
                    struct ast_s *body;
                };
            };
        };

        // Declaration
        struct {
            struct ast_s *declvar;
            struct ast_s *declinit;
        };

        // Array initializer
        list_t *arrayinit;

        // if statement or ternary operator
        struct {
            struct ast_s *cond;
            struct ast_s *then;
            struct ast_s *els;
        };

        // for statement
        struct {
            struct ast_s *forinit;
            struct ast_s *forcond;
            struct ast_s *forstep;
            struct ast_s *forbody;
        };

        // return statement
        struct ast_s *retval;

        // Compound statement
        list_t *stmts;

        // Struct reference
        struct {
            struct ast_s *struc;
                    char *field; // specific to ast_to_string only
        };
    };
} ast_t;

/**
 * @def get_priv
 * @brief
 *
 */
#define get_priv(tok, type)                                         \
    ({                                                              \
        assert(__builtin_types_compatible_p(typeof(tok), token_t)); \
        ((type) tok.priv);                                          \
    })

/**
 * @def get_ttype
 * @brief
 *
 */
#define get_ttype(tok)                                              \
    ({                                                              \
        assert(__builtin_types_compatible_p(typeof(tok), token_t)); \
        (tok.type);                                                 \
    })

/**
 * @def get_token
 * @brief
 *
 */
#define get_token(tok, ttype, priv_type) \
    ({                                   \
        assert(get_ttype(tok) == ttype); \
        get_priv(tok, priv_type);        \
    })

/**
 * @def get_char
 * @brief
 *
 */
#define get_char(tok)    get_token(tok, TTYPE_CHAR, char)


/**
 * @def get_strtok
 * @brief
 *
 */
#define get_strtok(tok)  get_token(tok, TTYPE_STRING, char*)

/**
 * @def get_ident
 * @brief
 *
 */
#define get_ident(tok)   get_token(tok, TTYPE_IDENT, char*)

/**
 * @def get_number
 * @brief
 *
 */
#define get_number(tok)  get_token(tok, TTYPE_NUMBER, char*)

/**
 * @def get_punct
 * @brief
 *
 */
#define get_punct(tok)   get_token(tok, TTYPE_PUNCT, int)

#endif /* C_STACKVM_H */
