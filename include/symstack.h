/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Stack for symbols used in precedence analysis
 * @date 2023-10-15
 */

#ifndef SYMSTACK_H
#define SYMSTACK_H
#include <stdbool.h>
#include <stdlib.h>
#include "lexical_analyzer.h"

#define MAX_TOKEN_SYMBOL_SIZE 4
typedef struct DATA
{
    token_T token;
    bool isTerminal;
    bool isHandleBegin;
    char symbol[MAX_TOKEN_SYMBOL_SIZE];
} symstack_data_t;
typedef struct NODE
{
    symstack_data_t data;
    struct NODE *previous;
} node_t;

typedef struct SYMSTACK
{
    node_t *top;
    size_t size;
} symstack_t;

// stack functions
// init
void init_symstack(symstack_t *stack);

// push
bool symstack_push(symstack_t *stack, symstack_data_t data);

// pop
symstack_data_t symstack_pop(symstack_t *stack);

// peek
symstack_data_t symstack_peek(const symstack_t *stack);

bool symstack_is_empty(const symstack_t *stack);

// dispose
bool symstack_dispose(symstack_t *stack);

// convert token type to symbol
const char *convert_token_type_to_string(token_T token);

/* debugging functions */
void print_line(unsigned int width);
void print_node(node_t *node, unsigned int width);
void print_stack(const symstack_t *stack, unsigned int width);
#endif
