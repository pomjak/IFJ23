/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 1
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
    symtable_init(&global_sym_table);
    return 0;
}