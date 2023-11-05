/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 06 for symstack handle stack like a fool
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symstack.h"
#include <assert.h>

int main()
{
    symstack_t stack=NULL;
    unsigned int error = 0;
    init_symstack(&stack);

    pop_scope(&stack, &error);
    assert(error == SYMTAB_NOT_INITIALIZED);

    dispose_stack(&stack, &error);
    assert(error == SYMTAB_OK);

    return 0;
}