/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Stack for symbols used in precedence analysis
 * @date 2023-10-15
 */

#include <stdio.h>
#include <stdlib.h>
#include "symstack.h"

void init_symstack(symstack_t *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

// push
bool symstack_push(symstack_t *stack, data_t data)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        return false;
    }

    new_node->data = data;
    new_node->previous = stack->top;
    stack->top = new_node;
    stack->size += 1;
    return true;
}

// pop
data_t symstack_pop(symstack_t *stack)
{
    data_t old_node_data;
    if (!symstack_isEmpty(stack))
    {
        node_t *old_node = stack->top;
        old_node_data = old_node->data;

        stack->top = old_node->previous;
        stack->size -= 1;
        free(old_node);
        return old_node_data;
    }
    return old_node_data;
}

// peek
data_t symstack_peek(const symstack_t *stack)
{
    return stack->top->data;
}

bool symstack_isEmpty(const symstack_t *stack)
{
    return stack->size == 0 && stack->top == NULL;
}

// dispose
bool symstack_dispose(symstack_t *stack)
{
    while (!symstack_isEmpty(stack))
    {
        symstack_pop(stack);
    }
    return symstack_isEmpty(stack);
}

void print_line(unsigned int width)
{
    printf("+");
    for (int i = 1; i < width - 1; i++)
    {
        printf("-");
    }
    printf("+\n");
}

void print_node(node_t *node, unsigned int width)
{
    print_line(width);
    printf("| ");
    for (int i = 2; i < width - 2; i++)
    {
        if (i == (int)(width / 2))
            printf("%c", node->data.c);
        else
            printf(" ");
    }
    printf(" |\n");
}

void print_stack(const symstack_t *stack, unsigned int width)
{
    node_t nodes[stack->size];

    node_t *current_node = stack->top;
    for (int i = 0; i < stack->size; i++)
    {
        nodes[i] = *current_node;
        current_node = current_node->previous;
    }

    for (int i = 0; i < stack->size; i++)
    {
        print_node(&nodes[i], width);
    }

    // print end
    print_line(width);
}
