#ifndef SCANNER_H
#define SCANNER_H

typedef enum scanner_state {
    START,
    ERROR
} scanner_state_t;

/**
 * @brief defines type of token
 * 
 */
typedef enum token_type
{
    INT,
    FLOAT,
    STRING
} token_type_t;

/**
 * @brief value of token
 * 
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
    token_type_t type;
    token_value_t value;
}token_t;

#endif
