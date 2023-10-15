#ifndef SYMSTACK_H
#define SYMSTACK_H
#include <stdbool.h>

typedef struct DATA
{
    int id; // data struct here
    char c;
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
void init_stack(symstack_t *stack);

// push
bool symstack_push(symstack_t *stack, data_t data);

// pop
data_t symstack_pop(symstack_t *stack);

// peek
data_t symstack_peek(const symstack_t *stack);

bool symstack_isEmpty(const symstack_t *stack);

// dispose
bool symstack_dispose(symstack_t *stack);

/* debugging functions */
void print_line(unsigned int width);
void print_node(node_t *node, unsigned int width);
void print_stack(const symstack_t *stack, unsigned int width);
#endif
