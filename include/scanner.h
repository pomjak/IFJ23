#ifndef SCANNER_H
#define SCANNER_H

#include "dyn_string.h"

/**
 * @brief defines scanner states
 */
typedef enum scanner_state {
    SCANNER_STATE_START,
    SCANNER_STATE_ERROR,
    SCANNER_STATE_EOF,
    SCANNER_STATE_EOL,

    // data type states
    SCANNER_STATE_NUMBER
} scanner_state_t;

/**
 * @brief defines type of tokenxw
 */
typedef enum token_data_t
{
    SCANNER_TOKEN_UNDEFINED,
    SCANNER_TOKEN_INT,
    SCANNER_TOKEN_DOUBLE,
    SCANNER_TOKEN_DOUBLE_EXP,
    SCANNER_TOKEN_STRING,
    SCANNER_TOKEN_EMPTY_STRING,
    SCANNER_TOKEN_EOF
} token_data_t;

/**
 * @brief value of tokenxw
 */
typedef union token_value
{
    float floatval;
    int intval;
    dstring_t string; 
    // keyword
    // data_type
}token_value_t;

/**
 * @brief token type 
 */
typedef struct token
{
    token_data_t type;
    token_value_t value;
}token_t;

int get_token(token_t * token);

#endif
