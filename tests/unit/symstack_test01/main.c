/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stid.fit.vutbr.cz
 * @brief   test 01 for symstack [init]
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
    symstack_t local_table = NULL;
    unsigned int error;
    init_symstack(&local_table);

    add_scope(&local_table, &error);
    assert(error == SYMTAB_OK);

    dispose_stack(&local_table, &error);
    assert(error == SYMTAB_OK);

    return 0;
}