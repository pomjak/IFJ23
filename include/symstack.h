/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Stack for symbols used in precedence analysis
 * @date 2023-10-15
 */

#ifndef SYMSTACK_H
#define SYMSTACK_H
#include <stdbool.h>

typedef struct DATA
{
    int id; // data struct here
    const char *c;
    // token_T token;
    // bool isTerminal;
    // bool isHandleBegin;
    // char symbol;
} data_t;
typedef struct NODE
{
    data_t data;
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
bool symstack_push(symstack_t *stack, data_t data);

// pop
data_t symstack_pop(symstack_t *stack);

// peek
data_t symstack_peek(const symstack_t *stack);

bool symstack_is_empty(const symstack_t *stack);

// dispose
bool symstack_dispose(symstack_t *stack);

/* debugging functions */
void print_line(unsigned int width);
void print_node(node_t *node, unsigned int width);
void print_stack(const symstack_t *stack, unsigned int width);
#endif
