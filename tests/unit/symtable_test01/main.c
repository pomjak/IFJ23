/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 1 empty symtable [search]
 * @version 0.1
 * @date 2023-10-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "symtable.h"
#include <assert.h>

int main()
{   
    symtab_t global_sym_table;
    dstring_t item1;
    symtable_init(&global_sym_table);
    dstring_init(&item1);
    dstring_add_const_str(&item1,"item1");


    assert(symtable_search(&global_sym_table,&item1) == NULL);
    dstring_free(&item1);
    symtable_dispose(&global_sym_table);
    
    return 0;
}