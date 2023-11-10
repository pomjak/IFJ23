/**
 * @file symstack.h
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief   Declarations of functions of symstack
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include "symtable.h"

typedef struct symstack_element
{
    symtab_t *local_sym;
    struct symstack_element *next;

}*symstack_t;

/**
 * @brief init symstack
 * 
 * @param first ptr to stack [use in argument as "&stack"]
 */
void init_symstack(symstack_t *first);

/**
 * @brief adds local symtable for new scope
 *
 * @param first ptr to stack [use in argument as "&stack"]
 * @param error error flag from symtab
 */
void add_scope(symstack_t *first, unsigned int *error);

/**
 * @brief removes first[last added] stacked local symtab
 *
 * @param first ptr to stack [use in argument as "&stack"]
 * @param error error code
 */
void pop_scope(symstack_t *first, unsigned int *error);

/**
 * @brief dispose of the stack
 *
 * @param first ptr to stack [use in argument as "&stack"]
 * @param error error code
 */
void dispose_stack(symstack_t *first, unsigned int *error);

/**
 * @brief searches symbol in the whole stack of symtabs
 *
 * @param stack ptr to stack [use in argument as only "stack"]
 * @param id    id to search for
 * @param error error code
 * @return ptr to found data
 */
symtab_item_t *search_stack(symstack_t stack, dstring_t *id, unsigned int *error);

/**
 * @brief peek to stack and find out if it is safe to use stack->local_sym (e.g. with  symtable_insert)
 * otherwise if stack is NULL segmentation fault occurs
 *
 * @param stack ptr to stack [use in argument as "stack"]
 * @return true stack is not null -> safe to use
 * @return false stack is null -> skip
 */
bool peek_stack(symstack_t stack);