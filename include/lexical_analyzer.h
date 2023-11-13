/**
 * @name IFJ23
 * @file lexical_analyzer.h
 * @brief Lexical analyzer header
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @author Jakub Pomsar <xpomsa00@stud.fit.vutbr.cz>
 * @date 06.10.2023
 **/

#ifndef SCANNER_H
#define SCANNER_H

#include "dyn_string.h"

/**
 * @brief defines type of tokens
 */
typedef enum {
    TOKEN_UNDEFINED,
    TOKEN_EOF,
    TOKEN_UND_SCR,
    TOKEN_COMMA,
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
    TOKEN_SEMICOL,
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
    TOKEN_DT_DOUBLE,
    TOKEN_DT_INT,
    TOKEN_DT_STRING,
    TOKEN_ELSE,
    TOKEN_FUNC,
    TOKEN_IF,
    TOKEN_LET,
    TOKEN_NIL,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_WHILE,

    TOKEN_INT,
    TOKEN_DBL,

    TOKEN_STRING
} token_type_T;

/**
 * @brief value of tokens
 */
typedef union
{
    double     double_val;
    int        int_val;
    dstring_t  string_val;
    bool       is_nilable;
} token_value_T;

/**
 * @brief token type 
 */
typedef struct
{
    bool preceding_eol;
    token_type_T  type;
    token_value_T value;
} token_T;

typedef void state_T;

int get_token(token_T *token);
void print_token(token_T token);

#endif
