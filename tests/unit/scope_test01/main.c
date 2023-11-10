/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 01 for scope [init, add scope, dispose scopes]
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
    scope_t local_table;
    unsigned int error;
    init_scope(&local_table);

    add_scope(&local_table, &error);
    assert(error == SYMTAB_OK);

    dispose_scope(&local_table, &error);
    assert(error == SYMTAB_OK);

    return 0;
}