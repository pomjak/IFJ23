#ifndef SCANNER_H
#define SCANNER_H

#include "dyn_string.h"

/**
 * @brief defines type of tokenxw
 */
typedef enum {
    TOKEN_UNDEFINED,
    TOKEN_EOL,
    TOKEN_UND_SCR,
    TOKEN_LT,
    TOKEN_LEQ,
    TOKEN_GT,
    TOKEN_GEQ,
    TOKEN_NOT_NIL,
    TOKEN_NEQ,
    TOKEN_NIL_CHECK,
    TOKEN_ASS,
    TOKEN_EQ,
    TOKEN_COL,
    TOKEN_R_BKT,
    TOKEN_L_BKT,
    TOKEN_R_PAR,
    TOKEN_L_PAR,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_RET_VAL,
    TOKEN_MUL,
    TOKEN_DIV,

    TOKEN_IDENTIFIER,

    TOKEN_INT,
    TOKEN_DBL,
    TOKEN_EXP
} token_type_T;

/**
 * @brief value of tokenxw
 */
typedef union
{
    float     float_val;
    int       int_val;
    dstring_t string_val;
} token_value_T;

/**
 * @brief token type 
 */
typedef struct
{
    token_type_T  type;
    token_value_T value;
} token_T;

typedef void state_T;

int get_token(token_T *token);

#endif
