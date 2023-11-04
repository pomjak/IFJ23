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
}

void add_scope(symstack_t *first, unsigned int *error)
{
    if(!first)
    {
        symstack_t *new = malloc(sizeof(symstack_t));
        new->next = NULL;
        new->local_sym = create_local_symtab(error);
    }
}