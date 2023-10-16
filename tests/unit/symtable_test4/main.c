/**
 * @file main.c (unit test 4 )
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 4
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item, value1;

    symtable_init(&global_sym_table);

    dstring_init(&item);

    dstring_init(&value1);

    

    dstring_free(&item);

    dstring_free(&value1);

    symtable_dispose(&global_sym_table);

    return 0;
}