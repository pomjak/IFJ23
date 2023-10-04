#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lexical_analyzer.h"
#include "error.h"
#include "debug.h"

/* For set next state */
#define NEXT_STATE(X) {actual_state = X; return;}
#define UNREAD(X)     unreaded = X

/* Actual state */
state_T (*actual_state)(char);
bool is_final = false;
char unreaded = '\0';
token_T actual_token;

/**
 * Definition of states 
 */
state_T start(char);
state_T eol(char);
state_T und_scr(char);
state_T lt(char);
state_T leq(char);
state_T gt(char);
state_T geq(char);
state_T not_nil(char);
state_T neq(char);
state_T qm(char);
state_T nil_check(char);
state_T ass(char);
state_T eq(char);
state_T col(char);
state_T r_bkt(char);
state_T l_bkt(char);
state_T r_par(char);
state_T l_par(char);
state_T add(char);
state_T sub(char);
state_T ret_val(char);
state_T mul(char);
state_T s_div(char);

state_T identifier(char);

state_T line_c(char);

state_T s_int(char);
state_T dbl_s(char);
state_T dbl(char);
state_T exp_s(char);
state_T exp_sign(char);
state_T s_exp(char);


bool is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

bool is_number(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alfa_num(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool is_alfa(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

const token_type_T tokens[] = {
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
};

const char* tokens_names[] = {
    "\n",
    "<UND_SCR>",
    "<LT>",
    "<LEQ>",
    "<GT>",
    "<GEQ>",
    "<NOT_NIL>",
    "<NEQ>",
    "<NIL_CHECK>",
    "<ASS>",
    "<EQ>",
    "<COL>",
    "<R_BKT>",
    "<L_BKT>",
    "<R_PAR>",
    "<L_PAR>",
    "<ADD>",
    "<SUB>",
    "<RET_VAL>",
    "<MUL>",
    "<DIV>",

    "<IDENTIFIER>",

    "<INT>",
    "<DBL>",
    "<EXP>"
};

const int tokens_count = 25;

void print_token(token_T token) {

    for (int i = 0; i < tokens_count; i++) {
        if (token.type == tokens[i]) {
            printf("%s", tokens_names[i]);
        }
    }
}

/**
 * @brief Get the token object
 *
 * @param token
 * @return int return code
 */
int get_token(token_T *token)
{
    DEBUG_PRINT("get token");
    actual_token.type   = TOKEN_UNDEFINED;

    is_final     = false;
    actual_state = start;

    char readed;

    if (unreaded == '\0') {
        readed = fgetc(stdin);
    } else {
        readed = unreaded;
        unreaded = '\0';
    }
    
    while(readed != EOF) {
        actual_state(readed);

        if (actual_state == NULL) break;

        if (unreaded == '\0') {
            readed = fgetc(stdin);
        } else {
            readed = unreaded;
            unreaded = '\0';
        }
    }

    token->type = actual_token.type;
    print_token(*token);

    return 0;
}

state_T start(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_whitespace(readed))  NEXT_STATE(start);
    if (is_number(readed))      NEXT_STATE(s_int);
    if (readed == '\n')         NEXT_STATE(eol);
    if (readed == '_')          NEXT_STATE(und_scr);
    if (readed == '<')          NEXT_STATE(lt);
    if (readed == '>')          NEXT_STATE(gt);
    if (readed == '!')          NEXT_STATE(not_nil);
    if (readed == '?')          NEXT_STATE(qm);
    if (readed == '=')          NEXT_STATE(ass);
    if (readed == ':')          NEXT_STATE(col);
    if (readed == '}')          NEXT_STATE(r_bkt);
    if (readed == '{')          NEXT_STATE(l_bkt);
    if (readed == ')')          NEXT_STATE(r_par);
    if (readed == '(')          NEXT_STATE(l_par);
    if (readed == '+')          NEXT_STATE(add);
    if (readed == '-')          NEXT_STATE(sub);
    if (readed == '*')          NEXT_STATE(mul);
    if (readed == '/')          NEXT_STATE(s_div);
    if (is_alfa(readed))        NEXT_STATE(identifier);

    NEXT_STATE(NULL);
}

/**
 * Operators
 */

state_T eol(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_EOL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T und_scr(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_alfa_num(readed))  NEXT_STATE(identifier);
    
    is_final          = true;
    actual_token.type = TOKEN_UND_SCR;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T lt(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '=')  NEXT_STATE(leq);

    is_final          = true;
    actual_token.type = TOKEN_LT;

    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T leq(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_LEQ;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T gt(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '=')  NEXT_STATE(geq);

    is_final          = true;
    actual_token.type = TOKEN_GT;

    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T geq(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_GEQ;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T not_nil(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '=')  NEXT_STATE(neq);
    
    is_final          = true;
    actual_token.type = TOKEN_NOT_NIL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T neq(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_NEQ;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T qm(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '?')  NEXT_STATE(nil_check);
    
    NEXT_STATE(NULL);   
}

state_T nil_check(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_NIL_CHECK;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T ass(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '=')  NEXT_STATE(eq);
    
    is_final          = true;
    actual_token.type = TOKEN_ASS;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T eq(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_EQ;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T col(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_COL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T r_bkt(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_R_BKT;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T l_bkt(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_L_BKT;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T r_par(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_R_PAR;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T l_par(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_L_PAR;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T add(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_ADD;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T sub(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '>')  NEXT_STATE(ret_val);
    
    is_final          = true;
    actual_token.type = TOKEN_SUB;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T ret_val(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_RET_VAL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T mul(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_MUL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T s_div(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed == '/') NEXT_STATE(line_c);

    is_final          = true;
    actual_token.type = TOKEN_DIV;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T line_c(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed != '\n') NEXT_STATE(line_c);

    NEXT_STATE(start);   
}

/**
 * Keywords/Identifiers
 */

state_T identifier(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (is_alfa_num(readed))  NEXT_STATE(identifier);

    is_final          = true;
    actual_token.type = TOKEN_IDENTIFIER;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

/**
 * Number literal
 */

state_T s_int(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_number(readed))                  NEXT_STATE(s_int);
    if (readed == '.')                      NEXT_STATE(dbl_s);
    if (readed == 'e' || readed == 'E')     NEXT_STATE(exp_s);
    
    is_final          = true;
    actual_token.type = TOKEN_INT;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T dbl_s(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_number(readed))      NEXT_STATE(dbl);

    NEXT_STATE(NULL);   
}

state_T dbl(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_number(readed))                   NEXT_STATE(dbl);
    if (readed == 'e' || readed == 'E')      NEXT_STATE(exp_s);
    
    is_final          = true;
    actual_token.type = TOKEN_DBL;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T exp_s(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '+' || readed == '-')      NEXT_STATE(exp_sign);
    if (is_number(readed))                   NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T exp_sign(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_number(readed))  NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T s_exp(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_number(readed))  NEXT_STATE(s_exp);
    
    is_final          = true;
    actual_token.type = TOKEN_EXP;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}