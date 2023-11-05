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
 * @param first 
 */
void init_symstack(symstack_t *first);

/**
 * @brief adds local symtable for new scope
 * 
 * @param first ptr to stack
 * @param error error flag from symtab
 */
void add_scope(symstack_t *first, unsigned int *error);

/**
 * @brief removes first stacked local symtab
 * 
 * @param first ptr to stack
 * @param error error code
 */
void pop_scope(symstack_t *first, unsigned int *error);

/**
 * @brief dispose of stack
 * 
 * @param first ptr to stack
 * @param error error code 
 */
void dispose_stack(symstack_t *first, unsigned int *error);

/**
 * @brief searches symbol over whole stack of symtabs
 * 
 * @param stack ptr to stack
 * @param error error code 
 */
void search_stack(symstack_t *stack, unsigned int *error);