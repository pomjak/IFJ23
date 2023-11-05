/**
 * @file symstack.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief   Implementation of stack for local symbol tables
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symstack.h"

void init_symstack(symstack_t *first)
{
    first = NULL;
    return;
}

void add_scope(symstack_t *first, unsigned int *error)
{

    symstack_t new = malloc(sizeof(struct symstack_element));

    if (!new)
    {
        report_error(error, ERR_INTERNAL);
        return;
    }

    new->next = *first;
    new->local_sym = create_local_symtab(error);
    *first = new;
}

void pop_scope(symstack_t *first, unsigned int *error)
{
    symstack_t temp;
    temp = (*first)->next;

    symtable_dispose((*first)->local_sym);
    free((*first)->local_sym);

    free(*first);
    (*first) = temp;
}

void dispose_stack(symstack_t *first, unsigned int *error)
{
    symstack_t temp;
    if (*first == NULL)
        return;

    while (*first)
    {
        temp = (*first)->next;

        symtable_dispose((*first)->local_sym);
        free((*first)->local_sym);

        free(*first);
        (*first) = temp;
    }
    first = NULL;
}

void search_stack(symstack_t *stack, unsigned int *error)
{
    
}