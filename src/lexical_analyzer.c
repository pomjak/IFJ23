/**
 * @name IFJ23
 * @file lexical_analyzer.c
 * @brief Lexical analyzer
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @author Jakub Pomsar <xpomsa00@stud.fit.vutbr.cz>
 * @date 20.11.2023
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
#define UNREAD(X)     unread = X

/* Actual state */
state_T (*actual_state)(char);

bool is_final = false;
bool set_eol  = false;//flag for preceding eol 

char unread = '\0';

token_T actual_token;

unsigned actual_line = 1;
unsigned actual_column = 0;

int open_comments = 0;
int hexa_string = 0;

bool malloc_error = false;

dstring_t read_string;

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

#define indentation_ok          0
#define indentation_fail        1
#define indentation_memory_fail 2
unsigned indentation_perform(dstring_t *read_string) {
    
    dstring_t indent;
    dstring_t result;

    if(!dstring_init(&indent)) return indentation_memory_fail;
    if(!dstring_init(&result)) return indentation_memory_fail;

    for (size_t i = read_string->length - 3; i-- > 0;) {
        if (read_string->str[i] == '\n') break;
        if (!dstring_append(&indent, read_string->str[i])) return indentation_memory_fail;
    }

    unsigned indent_position = 0;
    size_t  for_iter = (read_string->length < indent.length + 4) ? 0 : read_string->length - indent.length - 4;

    for (size_t i = 0; i < for_iter; i++) {
        
        if (read_string->str[i] == '\n') {
            if (indent_position < indent.length && indent_position != 0) return indentation_fail;

            indent_position = 0;
            
            if (!dstring_append(&result, read_string->str[i])) return indentation_memory_fail;
            continue;
        }

        if (indent_position < indent.length) {
            if (read_string->str[i] != indent.str[indent.length - indent_position - 1]) return indentation_fail;
        } else {
            if (!dstring_append(&result, read_string->str[i])) return indentation_memory_fail;
        }

        indent_position++;
    }

    dstring_clear(read_string);
    if (!dstring_copy(&result, read_string)) return indentation_memory_fail;

    dstring_free(&indent);
    dstring_free(&result);

    return indentation_ok;
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

const char *tokens_names[] = {
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

    "<STRING "};

const int tokens_count = 39;

void print_token(token_T token) {

    for (int i = 0; i < tokens_count; i++) {
        if (token.type == tokens[i]) {
            if(token.preceding_eol){
                printf("\n");
            }
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
    actual_token.preceding_eol = false;

    set_eol        = false;
    is_final       = false;
    actual_state   = start;
    if(!dstring_init(&read_string)) {
        return ERR_INTERNAL;
    }
    if(!dstring_init(&tmp_string)) {
        return ERR_INTERNAL;
    }

    char read;
    
    do {
        if (unread == '\0') {
            read = fgetc(stdin);
            actual_column++;

            if (read == '\n') {
                actual_line++;
                actual_column =1;
            }
        } else {
            read = unread;
            unread = '\0';
        }

        actual_state(read);

        if (malloc_error == true) return ERR_INTERNAL;
        if (actual_state == NULL) break;
    } while(read != EOF);

    dstring_free(&tmp_string);

    if (actual_token.type != TOKEN_STRING && actual_token.type != TOKEN_IDENTIFIER) {
        dstring_free(&read_string);
    }

    if (open_comments > 0) {
        ERROR_PRINT("Block comment is not closed.");
    }
    //if (hexa_string == 0)  ERROR_PRINT("Hexa is empty in \\u{}.");
    if (hexa_string >  8) {
        ERROR_PRINT("Hexa is too long in \\u{}.");
    }

    if (!is_final) {
        ERROR_PRINT("Lexical analyzer did not end in final state. On line %d and column %d.", actual_line, actual_column);

        return ERR_LEXICAL;
    }

    token->preceding_eol = actual_token.preceding_eol;
    token->type  = actual_token.type;
    token->value = actual_token.value;

    return ERR_NO_ERR;
}

state_T start(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (is_whitespace(read))  NEXT_STATE(start);
    if (is_number(read)) {
        if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(s_int);
    }
    if (read == '\n'){        set_eol = true; NEXT_STATE(start);}
    if (read == EOF)          eof(read);
    if (read == '_') {
        if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(und_scr);
    }
    if (read == ',')          NEXT_STATE(comma);
    if (read == '<')          NEXT_STATE(lt);
    if (read == '>')          NEXT_STATE(gt);
    if (read == '!')          NEXT_STATE(not_nil);
    if (read == '?')          NEXT_STATE(qm);
    if (read == '=')          NEXT_STATE(ass);
    if (read == ':')          NEXT_STATE(col);
    if (read == ';')          NEXT_STATE(semicol);
    if (read == '}')          NEXT_STATE(r_bkt);
    if (read == '{')          NEXT_STATE(l_bkt);
    if (read == ')')          NEXT_STATE(r_par);
    if (read == '(')          NEXT_STATE(l_par);
    if (read == '+')          NEXT_STATE(add);
    if (read == '-')          NEXT_STATE(sub);
    if (read == '*')          NEXT_STATE(mul);
    if (read == '/')          NEXT_STATE(s_div);
    if (is_alfa(read)) {
        if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(identifier);
    }
    if (read == '"')          NEXT_STATE(string_start);

    NEXT_STATE(NULL);
}

/**
 * Operators
 */

state_T eof(char read) {
    UNUSED(read);
    DEBUG_PRINT("read char is %c", read);

    is_final          = true;
    actual_token.preceding_eol = set_eol;//set eol flag if eol was found before token
    set_eol = false;   
    actual_token.type = TOKEN_EOF;

    NEXT_STATE(NULL);   
}

state_T und_scr(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_alfa_num(read))  NEXT_STATE(identifier);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_UND_SCR;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T comma(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_COMMA;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T lt(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '=')  NEXT_STATE(leq);

    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_LT;

    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T leq(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_LEQ;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T gt(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '=')  NEXT_STATE(geq);

    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_GT;

    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T geq(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_GEQ;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T not_nil(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '=')  NEXT_STATE(neq);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_NOT_NIL;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T neq(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_NEQ;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T qm(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '?')  NEXT_STATE(nil_check);
    
    NEXT_STATE(NULL);   
}

state_T nil_check(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_NIL_CHECK;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T ass(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '=')  NEXT_STATE(eq);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_ASS;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T eq(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_EQ;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T col(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_COL;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T semicol(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_SEMICOL;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T r_bkt(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_R_BKT;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T l_bkt(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_L_BKT;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T r_par(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_R_PAR;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T l_par(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_L_PAR;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T add(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_ADD;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T sub(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '>')  NEXT_STATE(ret_val);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_SUB;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T ret_val(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_RET_VAL;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T mul(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_MUL;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T s_div(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (read == '/') NEXT_STATE(line_c);
    if (read == '*') {
        open_comments++;
        NEXT_STATE(block_c);
    }

    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_DIV;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

/**
 * Comments
 */

state_T line_c(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (read == EOF) eof(read);
    if (read != '\n'){
        set_eol = true;
        NEXT_STATE(line_c);
        }

    NEXT_STATE(start);   
}

state_T block_c(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (read == '*')   NEXT_STATE(block_c_end_q);
    if (read == '/')   NEXT_STATE(block_c_start_q);
    if (read != '*')  NEXT_STATE(block_c);

    NEXT_STATE(NULL);   
}

state_T block_c_end_q(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (read == '/') {
        open_comments--;
        DEBUG_PRINT("Open comments %d", open_comments);

        if (is_last())  NEXT_STATE(start);
    }

    NEXT_STATE(block_c);
}

state_T block_c_start_q(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (read == '*')  open_comments++;
    DEBUG_PRINT("Open comments %d", open_comments);

    NEXT_STATE(block_c);  
}

/**
 * Keywords/Identifiers
 */

state_T identifier(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_alfa_num(read) || read == '_')  NEXT_STATE(identifier);

    is_final          = true;
    actual_token.type = TOKEN_IDENTIFIER;
    UNREAD(read);
    dstring_retract(&read_string, 1);
    actual_token.value.string_val = read_string;

    DEBUG_PRINT("read string is %s", read_string.str);

    for (int i = 0; i < keywords_count; i++) {
        if (dstring_cmp_const_str(&read_string, keywords[i]) == 0) {
            actual_token.type = keywords_tokens[i];
        }
    }

    for (int i = 0; i < data_types_count; i++) {
        if (dstring_cmp_const_str(&read_string, data_types[i]) == 0) {
            actual_token.type = data_types_tokens[i];
            actual_token.preceding_eol = set_eol;
            set_eol = false;
            actual_token.value.is_nilable = false;
            NEXT_STATE(data_type);
        }
    }

    actual_token.preceding_eol = set_eol;
    set_eol = false;

    NEXT_STATE(NULL);   
}

state_T data_type(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final = true;
    if(read == '?') actual_token.value.is_nilable = true;
    else              UNREAD(read);

    NEXT_STATE(NULL);
}

/**
 * Number literal
 */

state_T s_int(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_number(read))                  NEXT_STATE(s_int);
    if (read == '.')                      NEXT_STATE(dbl_s);
    if (read == 'e' || read == 'E')     NEXT_STATE(exp_s);

    is_final = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_INT;
    actual_token.value.int_val = atoi(dstring_to_str(&read_string));
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T dbl_s(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_number(read))      NEXT_STATE(dbl);

    NEXT_STATE(NULL);   
}

state_T dbl(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_number(read))                   NEXT_STATE(dbl);
    if (read == 'e' || read == 'E')      NEXT_STATE(exp_s);
    
    is_final          = true;
    actual_token.type = TOKEN_DBL;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.value.double_val = atof(dstring_to_str(&read_string));
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T exp_s(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read == '+' || read == '-')      NEXT_STATE(exp_sign);
    if (is_number(read))                   NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T exp_sign(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_number(read))  NEXT_STATE(s_exp);

    NEXT_STATE(NULL);   
}

state_T s_exp(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (is_number(read))  NEXT_STATE(s_exp);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_DBL;

    char* exponent;
    double base = strtod(dstring_to_str(&read_string), &exponent);
    exponent++;
    actual_token.value.double_val = base * pow(10, atof(exponent));
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

/**
 * String literal
 */

state_T string_start(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '"')                                 NEXT_STATE(empty_string);
    if (read == '\\')                                NEXT_STATE(string_escape);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read != '\\' && is_in_string_sigma(read))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T empty_string(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '"')  NEXT_STATE(eol_start_q);

    DEBUG_PRINT("read string is %s", read_string.str);
    
    is_final          = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type = TOKEN_STRING;
    actual_token.value.string_val = read_string;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T eol_start_q(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '\n')  NEXT_STATE(eol_end_q);

    NEXT_STATE(NULL);   
}

state_T m_string_inner(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '\\')                     NEXT_STATE(m_string_escape);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }
    
    if (read == '\n')                     NEXT_STATE(eol_end_q);
    if (read != '\\' && read != '\n')   NEXT_STATE(m_string_inner);

    NEXT_STATE(NULL);   
}

state_T m_string_escape(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == 'n') {
        if (!dstring_append(&read_string, '\n')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (read == 'r') {
        if (!dstring_append(&read_string, '\r')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (read == 't') {
        if (!dstring_append(&read_string, '\t')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(m_string_inner);
    }

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read != 'u')  NEXT_STATE(m_string_inner);
    if (read == 'u')  NEXT_STATE(m_string_hexa_q);

    NEXT_STATE(NULL);   
}

state_T m_string_hexa_q(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '{') {
        hexa_string = 0;
        NEXT_STATE(m_string_hexa);
    }

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read != '{')  NEXT_STATE(m_string_inner);

    NEXT_STATE(NULL);   
}

state_T m_string_hexa(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '}' && hexa_string > 0) {
        dstring_retract(&read_string, 1);
        if (!dstring_append(&read_string, hexa_to_dec(&tmp_string))) {
            malloc_error = true;
            return;
        }
        dstring_clear(&tmp_string);
        NEXT_STATE(m_string_inner);
    }
    if (is_hexa(read)) {
        if (!dstring_append(&tmp_string, read)) {
            malloc_error = true;
            return;
        }
        hexa_string++;
        if (hexa_string > 8) NEXT_STATE(NULL);
        NEXT_STATE(m_string_hexa);
    }

    NEXT_STATE(NULL);   
}

state_T eol_end_q(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '\\')  NEXT_STATE(m_string_escape);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read == '\n' || is_whitespace(read))  NEXT_STATE(eol_end_q);
    if (read != '"')                          NEXT_STATE(m_string_inner);
    if (read == '"')                          NEXT_STATE(m_string_end1);

    NEXT_STATE(NULL);   
}

state_T m_string_end1(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read == '\n') NEXT_STATE(eol_end_q);
    if (read != '"')  NEXT_STATE(m_string_inner);
    if (read == '"')  NEXT_STATE(m_string_end2);

    NEXT_STATE(NULL);   
}

state_T m_string_end2(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read == '\n') NEXT_STATE(eol_end_q);
    if (read != '"')  NEXT_STATE(m_string_inner);
    if (read == '"')  {
        //dstring_retract(&read_string, 4);
        
        unsigned status = indentation_perform(&read_string);

        if (status == indentation_memory_fail) {
            malloc_error = true;
            return;
        } else if (status == indentation_fail) {
            NEXT_STATE(NULL); // parsing error invalid indent
        }

        NEXT_STATE(string_end);
    }

    NEXT_STATE(NULL);   
}

state_T string_end(char read) {
    DEBUG_PRINT("read char is %c", read);
    
    is_final                      = true;
    actual_token.preceding_eol = set_eol;
    set_eol = false;
    actual_token.type             = TOKEN_STRING;
    actual_token.value.string_val = read_string;
    
    UNREAD(read);
    NEXT_STATE(NULL);   
}

state_T string_inner(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '\\')                                NEXT_STATE(string_escape);
    if (read == '"')                                 NEXT_STATE(string_end);

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read != '\\' && is_in_string_sigma(read))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_escape(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == 'n') {
        if (!dstring_append(&read_string, '\n')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (read == 'r') {
        if (!dstring_append(&read_string, '\r')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (read == 't') {
        if (!dstring_append(&read_string, '\t')) {
            malloc_error = true;
            return;
        }
        NEXT_STATE(string_inner);
    }

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }

    if (read == 'u')                                                   NEXT_STATE(string_hexa_q);
    if ((read != 'u' && is_in_string_sigma(read)) || read == '"')  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_hexa_q(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '{') {
        hexa_string = 0;
        NEXT_STATE(string_hexa);
    }

    if (!dstring_append(&read_string, read)) {
            malloc_error = true;
            return;
        }
    
    if (read != '{' && is_in_string_sigma(read))  NEXT_STATE(string_inner);

    NEXT_STATE(NULL);   
}

state_T string_hexa(char read) {
    DEBUG_PRINT("read char is %c", read);

    if (read == '}' && hexa_string > 0) {
        dstring_retract(&read_string, 1);
        if (!dstring_append(&read_string, hexa_to_dec(&tmp_string))) {
            malloc_error = true;
            return;
        }
        dstring_clear(&tmp_string);
        NEXT_STATE(string_inner);
    }
    if (is_hexa(read)) {
        if (!dstring_append(&tmp_string, read)) {
            malloc_error = true;
            return;
        }
        hexa_string++;
        if (hexa_string > 8) NEXT_STATE(NULL);
        NEXT_STATE(string_hexa);
    }

    NEXT_STATE(NULL);   
}
