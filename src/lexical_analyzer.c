/**
 * @name IFJ23
 * @file lexical_analyzer.h
 * @brief Lexical analyzer header
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 08.10.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lexical_analyzer.h"
#include "error.h"
#include "debug.h"
#include <math.h> 

/* For set next state */
#define NEXT_STATE(X) {actual_state = X; return;}
#define UNREAD(X)     unreaded = X

/* Actual state */
state_T (*actual_state)(char);

bool is_final = false;
char unreaded = '\0';

token_T actual_token;

unsigned actual_line = 1;
unsigned actual_collumn = 0;

int open_comments = 0;
int hexa_string = 0;

bool malloc_error = false;

dstring_t readed_string;

dstring_t tmp_string;

/**
 * Definition of states 
 */
state_T start(char);
state_T eol(char);
state_T eof(char);
state_T und_scr(char);
state_T comma(char);
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
state_T semicol(char);
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
state_T data_type(char);

state_T line_c(char);
state_T block_c(char);
state_T block_c_end_q(char);
state_T block_c_start_q(char);

state_T s_int(char);
state_T dbl_s(char);
state_T dbl(char);
state_T exp_s(char);
state_T exp_sign(char);
state_T s_exp(char);

state_T string_start(char);
state_T empty_string(char);
state_T eol_start_q(char);
state_T m_string_inner(char);
state_T m_string_escape(char);
state_T m_string_hexa_q(char);
state_T m_string_hexa(char);
state_T eol_end_q(char);
state_T m_string_end1(char);
state_T m_string_end2(char);
state_T string_end(char);
state_T string_inner(char);
state_T string_escape(char);
state_T string_hexa_q(char);
state_T string_hexa(char);


bool is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

bool is_number(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alfa(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool is_alfa_num(char c) {
    return (is_alfa(c) || is_number(c));
}

bool is_last() {
    return open_comments == 0;
}

bool is_hexa(char c) {
    return ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || is_number(c));
}

bool is_in_string_sigma(char c) {
    return (c > 31 && c != '"');
}

char hexa_to_dec(dstring_t *hexa) {
    char tmp = 0;

    DEBUG_PRINT("hexa is %lu %s", hexa->length, hexa->str);

    for (size_t i = hexa->length > 1 ? hexa->length - 2 : 0; i < hexa->length; i++) {
        tmp = tmp << 4;
        char hexa_num = tolower(dstring_to_str(hexa)[i]);
        tmp |= is_number(hexa_num) ? hexa_num - '0' : hexa_num - 'a' + 10;
    }

    return tmp;
}

const char* keywords[] = {
    "else",
    "func", 
    "if",
    "let",
    "nil",
    "return",
    "var",
    "while"
};

const token_type_T keywords_tokens[] = {
    TOKEN_ELSE,
    TOKEN_FUNC,
    TOKEN_IF,
    TOKEN_LET,
    TOKEN_NIL,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_WHILE
};

const int keywords_count = 8;

const char* data_types[] = {
    "Double",
    "Int",
    "String"
};

const token_type_T data_types_tokens[] = {
    TOKEN_DT_DOUBLE,
    TOKEN_DT_INT,
    TOKEN_DT_STRING
};

const int data_types_count = 3;

const token_type_T tokens[] = {
    TOKEN_EOL,
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
};

const char* tokens_names[] = {
    "\n",
    "<EOF>",
    "<UND_SCR>",
    "<COMMA>",
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
    "<SEMICOL>",
    "<R_BKT>",
    "<L_BKT>",
    "<R_PAR>",
    "<L_PAR>",
    "<ADD>",
    "<SUB>",
    "<RET_VAL>",
    "<MUL>",
    "<DIV>",

    "<IDENTIFIER ",
    "<DT_DOUBLE ",
    "<DT_INT ",
    "<DT_STRING ",
    "<ELSE>",
    "<FUNC>",
    "<IF>",
    "<LET>",
    "<NIL>",
    "<RETURN>",
    "<VAR>",
    "<WHILE>",

    "<INT ",
    "<DBL ",

    "<STRING "
};

const int tokens_count = 39;

void print_token(token_T token) {

    for (int i = 0; i < tokens_count; i++) {
        if (token.type == tokens[i]) {
            if        (token.type == TOKEN_STRING) {
                printf("%s value='%s'>", tokens_names[i], token.value.string_val.str);
            } else if (token.type == TOKEN_INT) {
                printf("%s value='%d'>", tokens_names[i], token.value.int_val);
            } else if (token.type == TOKEN_DBL) {
                printf("%s value='%lf'>", tokens_names[i], token.value.double_val);
            } else if (token.type == TOKEN_IDENTIFIER) {
                printf("%s value='%s'>", tokens_names[i], token.value.string_val.str);
            } else if (token.type == TOKEN_DT_DOUBLE || token.type == TOKEN_DT_INT || token.type == TOKEN_DT_STRING) {
                printf("%s nilable='%d'>", tokens_names[i], token.value.is_nilable);
            } else {
                printf("%s", tokens_names[i]);
            }
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

    is_final       = false;
    actual_state   = start;
    if(!dstring_init(&readed_string)) {
        return ERR_INTERNAL;
    }
    if(!dstring_init(&tmp_string)) {
        return ERR_INTERNAL;
    }

    char readed;
    
    do {
        if (unreaded == '\0') {
            readed = fgetc(stdin);
            actual_collumn++;

            if (readed == '\n') {
                actual_line++;
                actual_collumn =1;
            }
        } else {
            readed = unreaded;
            unreaded = '\0';
        }

        actual_state(readed);

        if (malloc_error == true) return ERR_INTERNAL;
        if (actual_state == NULL) break;
    } while(readed != EOF);

    dstring_free(&tmp_string);

    if (open_comments > 0) {
        ERROR_PRINT("Block comment is not closed.");
    }
    //if (hexa_string == 0)  ERROR_PRINT("Hexa is empty in \\u{}.");
    if (hexa_string >  8) {
        ERROR_PRINT("Hexa is too long in \\u{}.");
    }

    if (!is_final) {
        ERROR_PRINT("Lexical analyzer did not end in final state. On line %d and collumn %d.", actual_line, actual_collumn);

        return ERR_LEXICAL;
    }

    token->type  = actual_token.type;
    token->value = actual_token.value;

    return ERR_NO_ERR;
}

state_T start(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (is_whitespace(readed))  NEXT_STATE(start);
    if (is_number(readed)) {
        if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(s_int);
    }
    if (readed == '\n')         NEXT_STATE(eol);
    if (readed == EOF)          eof(readed);
    if (readed == '_') {
        if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(und_scr);
    }
    if (readed == ',')          NEXT_STATE(comma);
    if (readed == '<')          NEXT_STATE(lt);
    if (readed == '>')          NEXT_STATE(gt);
    if (readed == '!')          NEXT_STATE(not_nil);
    if (readed == '?')          NEXT_STATE(qm);
    if (readed == '=')          NEXT_STATE(ass);
    if (readed == ':')          NEXT_STATE(col);
    if (readed == ';')          NEXT_STATE(semicol);
    if (readed == '}')          NEXT_STATE(r_bkt);
    if (readed == '{')          NEXT_STATE(l_bkt);
    if (readed == ')')          NEXT_STATE(r_par);
    if (readed == '(')          NEXT_STATE(l_par);
    if (readed == '+')          NEXT_STATE(add);
    if (readed == '-')          NEXT_STATE(sub);
    if (readed == '*')          NEXT_STATE(mul);
    if (readed == '/')          NEXT_STATE(s_div);
    if (is_alfa(readed)) {
        if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(identifier);
    }
    if (readed == '"')          NEXT_STATE(string_start);

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

state_T eof(char readed) {
    UNUSED(readed);
    DEBUG_PRINT("Readed char is %c", readed);

    is_final          = true;
    actual_token.type = TOKEN_EOF;

    NEXT_STATE(NULL);   
}

state_T und_scr(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_alfa_num(readed))  NEXT_STATE(identifier);
    
    is_final          = true;
    actual_token.type = TOKEN_UND_SCR;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T comma(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_COMMA;
    
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

state_T semicol(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final          = true;
    actual_token.type = TOKEN_SEMICOL;
    
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
    if (readed == '*') {
        open_comments++;
        NEXT_STATE(block_c);
    }

    is_final          = true;
    actual_token.type = TOKEN_DIV;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

/**
 * Comments
 */

state_T line_c(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed == EOF) eof(readed);
    if (readed != '\n') NEXT_STATE(line_c);

    NEXT_STATE(start);   
}

state_T block_c(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed != '*')  NEXT_STATE(block_c);
    if (readed == '*')   NEXT_STATE(block_c_end_q);
    if (readed == '/')   NEXT_STATE(block_c_start_q);


    NEXT_STATE(NULL);   
}

state_T block_c_end_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed == '/') {
        open_comments--;

        if (is_last())  NEXT_STATE(start);
    }

    NEXT_STATE(block_c);
}

state_T block_c_start_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (readed == '*')  open_comments++;

    NEXT_STATE(block_c);  
}

/**
 * Keywords/Identifiers
 */

state_T identifier(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_alfa_num(readed) || readed == '_')  NEXT_STATE(identifier);

    is_final          = true;
    actual_token.type = TOKEN_IDENTIFIER;
    UNREAD(readed);
    dstring_retract(&readed_string, 1);
    actual_token.value.string_val = readed_string;

    DEBUG_PRINT("Readed string is %s", readed_string.str);

    for (int i = 0; i < keywords_count; i++) {
        if (dstring_cmp_const_str(&readed_string, keywords[i]) == 0) {
            actual_token.type = keywords_tokens[i];
        }
    }

    for (int i = 0; i < data_types_count; i++) {
        if (dstring_cmp_const_str(&readed_string, data_types[i]) == 0) {
            actual_token.type = data_types_tokens[i];
            actual_token.value.is_nilable = false;
            NEXT_STATE(data_type);
        }
    }
    
    NEXT_STATE(NULL);   
}

state_T data_type(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final = true;

    if(readed == '?') actual_token.value.is_nilable = true;
    else              UNREAD(readed);

    NEXT_STATE(NULL);
}

/**
 * Number literal
 */

state_T s_int(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_number(readed))                  NEXT_STATE(s_int);
    if (readed == '.')                      NEXT_STATE(dbl_s);
    if (readed == 'e' || readed == 'E')     NEXT_STATE(exp_s);
    
    is_final          = true;
    actual_token.type = TOKEN_INT;
    actual_token.value.int_val = atoi(dstring_to_str(&readed_string));
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T dbl_s(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_number(readed))      NEXT_STATE(dbl);

    NEXT_STATE(NULL);   
}

state_T dbl(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_number(readed))                   NEXT_STATE(dbl);
    if (readed == 'e' || readed == 'E')      NEXT_STATE(exp_s);
    
    is_final          = true;
    actual_token.type = TOKEN_DBL;
    actual_token.value.double_val = atof(dstring_to_str(&readed_string));
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T exp_s(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed == '+' || readed == '-')      NEXT_STATE(exp_sign);
    if (is_number(readed))                   NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T exp_sign(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_number(readed))  NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T s_exp(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (is_number(readed))  NEXT_STATE(s_exp);
    
    is_final          = true;
    actual_token.type = TOKEN_DBL;

    char* exponent;
    double base = strtod(dstring_to_str(&readed_string), &exponent);
    exponent++;
    actual_token.value.double_val = base * pow(10, atof(exponent));
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

/**
 * String literal
 */

state_T string_start(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '"')                                 NEXT_STATE(empty_string);
    if (readed == '\\')                                NEXT_STATE(string_escape);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed != '\\' && is_in_string_sigma(readed))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T empty_string(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '"')  NEXT_STATE(eol_start_q);

    DEBUG_PRINT("Readed string is %s", readed_string.str);
    
    is_final          = true;
    actual_token.type = TOKEN_STRING;
    actual_token.value.string_val = readed_string;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T eol_start_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '\n')  NEXT_STATE(m_string_inner);

    NEXT_STATE(NULL);   
}

state_T m_string_inner(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '\\')                     NEXT_STATE(m_string_escape);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }
    
    if (readed == '\n')                     NEXT_STATE(eol_end_q);
    if (readed != '\\' && readed != '\n')   NEXT_STATE(m_string_inner);

    NEXT_STATE(NULL);   
}

state_T m_string_escape(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == 'n') {
        if (!dstring_append(&readed_string, '\n')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (readed == 'r') {
        if (!dstring_append(&readed_string, '\r')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (readed == 't') {
        if (!dstring_append(&readed_string, '\t')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed != 'u')  NEXT_STATE(m_string_inner);
    if (readed == 'u')  NEXT_STATE(m_string_hexa_q);

    NEXT_STATE(NULL);   
}

state_T m_string_hexa_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '{') {
        hexa_string = 0;
        NEXT_STATE(m_string_hexa);
    }

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed != '{')  NEXT_STATE(m_string_inner);

    NEXT_STATE(NULL);   
}

state_T m_string_hexa(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '}' && hexa_string > 0) {
        dstring_retract(&readed_string, 1);
        if (!dstring_append(&readed_string, hexa_to_dec(&tmp_string))) {
            malloc_error = true;
            return;
        }
        dstring_clear(&tmp_string);
        NEXT_STATE(m_string_inner);
    }
    if (is_hexa(readed)) {
        if (!dstring_append(&tmp_string, readed)) {
            malloc_error = true;
            return;
        }
        hexa_string++;
        if (hexa_string > 8) NEXT_STATE(NULL);
        NEXT_STATE(m_string_hexa);
    }

    NEXT_STATE(NULL);   
}

state_T eol_end_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed != '"')  NEXT_STATE(m_string_inner);
    if (readed == '"')  NEXT_STATE(m_string_end1);

    NEXT_STATE(NULL);   
}

state_T m_string_end1(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed == '\n') NEXT_STATE(eol_end_q);
    if (readed != '"')  NEXT_STATE(m_string_inner);
    if (readed == '"')  NEXT_STATE(m_string_end2);

    NEXT_STATE(NULL);   
}

state_T m_string_end2(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed == '\n') NEXT_STATE(eol_end_q);
    if (readed != '"')  NEXT_STATE(m_string_inner);
    if (readed == '"')  {
        dstring_retract(&readed_string, 4);
        NEXT_STATE(string_end);
    }

    NEXT_STATE(NULL);   
}

state_T string_end(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);
    
    is_final                      = true;
    actual_token.type             = TOKEN_STRING;
    actual_token.value.string_val = readed_string;
    
    UNREAD(readed);
    NEXT_STATE(NULL);   
}

state_T string_inner(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '\\')                                NEXT_STATE(string_escape);
    if (readed == '"')                                 NEXT_STATE(string_end);

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed != '\\' && is_in_string_sigma(readed))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_escape(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == 'n') {
        if (!dstring_append(&readed_string, '\n')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (readed == 'r') {
        if (!dstring_append(&readed_string, '\r')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (readed == 't') {
        if (!dstring_append(&readed_string, '\t')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }

    if (readed == 'u')                                                   NEXT_STATE(string_hexa_q);
    if ((readed != 'u' && is_in_string_sigma(readed)) || readed == '"')  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_hexa_q(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '{') {
        hexa_string = 0;
        NEXT_STATE(string_hexa);
    }

    if (!dstring_append(&readed_string, readed)) {
            malloc_error = true;
            return;
        }
    
    if (readed != '{' && is_in_string_sigma(readed))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_hexa(char readed) {
    DEBUG_PRINT("Readed char is %c", readed);

    if (readed == '}' && hexa_string > 0) {
        dstring_retract(&readed_string, 1);
        if (!dstring_append(&readed_string, hexa_to_dec(&tmp_string))) {
            malloc_error = true;
            return;
        }
        dstring_clear(&tmp_string);
        NEXT_STATE(string_inner);
    }
    if (is_hexa(readed)) {
        if (!dstring_append(&tmp_string, readed)) {
            malloc_error = true;
            return;
        }
        hexa_string++;
        if (hexa_string > 8) NEXT_STATE(NULL);
        NEXT_STATE(string_hexa);
    }

    NEXT_STATE(NULL);   
}