/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 1 empty symtable
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
    dstring_t temp;

    dstring_init(&temp);
    dstring_add_const_str(&temp,"x");

    symtable_init(&global_sym_table);
    
    assert(symtable_search(NULL,&temp) == NULL);
    assert(symtable_search(&global_sym_table,&temp) == NULL);
    
    return 0;
}