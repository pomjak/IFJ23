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

int main()
{   
    symtab_t global_sym_table;
    dstring_t *temp = NULL;

    dstring_init(temp);
    dstring_add_const_str(temp,"x");

    symtable_init(&global_sym_table);
    if(symtable_search(&global_sym_table,temp) != NULL)
        return 1;
        
    return 0;
}