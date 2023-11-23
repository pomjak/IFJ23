/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief   test 06 for scope foolproof mode
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "scope.h"
#include <assert.h>

int main()
{
    scope_t stack;
    unsigned int error;
    dstring_t temp;

    init_scope(&stack);
    dstring_init(&temp);
    dstring_add_const_str(&temp, "nope");

    if(peek_scope(stack))
        symtable_insert(stack->local_sym,&temp, &error);

    pop_scope(&stack, &error);
    assert(error == SYMTAB_NOT_INITIALIZED);

    search_scopes(stack, &temp, &error);
    assert(error == SYMTAB_NOT_INITIALIZED);

    for (int i = 0; i < 1024; i++)
    {
        add_scope(&stack, &error);
        symtable_insert(stack->local_sym, &temp, &error);
        assert(error == SYMTAB_OK);
    }

    dispose_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    search_scopes(stack, &temp, &error);
    assert(error == SYMTAB_NOT_INITIALIZED);

    dstring_free(&temp);

    return 0;
}