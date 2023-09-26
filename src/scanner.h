#ifndef SCANNER_H
#define SCANNER_H

/**
 * @brief defines scanner states
 */
typedef enum scanner_state {
    START,
    ERROR
} scanner_state_t;

/**
 * @brief defines type of tokenxw
 */
typedef enum token_data_t
{
    INT,
    FLOAT,
    STRING
} token_data_t;

/**
 * @brief value of tokenxw
 */
typedef union token_value
{
    float floatval;
    int intval;
    char * string; //dynamic string here
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
