/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00 
 * @brief   main for unit test 2 every setter & getter for 1 item
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
    dstring_t item1, value1;

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");
    dstring_init(&value1);
    dstring_add_const_str(&item1, "value1");

    symtable_init(&global_sym_table,11);

    symtable_insert(&global_sym_table, &item1);

    bool err;


    assert(set_type(&global_sym_table, &item1, function) == 0);

    assert(get_type(&global_sym_table, &item1, &err) == function);

    assert(err == false);


    assert(set_value(&global_sym_table, &item1, &value1) == 0);

    assert(dstring_cmp(get_value(&global_sym_table, &item1),&value1) == 0);


    assert(set_return_type(&global_sym_table, &item1, integer) == 0);
    
    assert(get_return_type(&global_sym_table, &item1, &err) == integer);

    assert(err == false);

    //item is function, mutability cannot be set
    assert(set_mutability(&global_sym_table, &item1, true) == 2);
    //mutability stays unset (unset from item_init)
    assert(get_mutability(&global_sym_table, &item1, &err) == false);
    //item is function so err flag is raised
    assert(err == true);


    assert(set_func_definition(&global_sym_table, &item1, true) == 0);
    
    assert(get_func_definition(&global_sym_table, &item1, &err) == true);

    assert(err == false);

    //again item is function, not var
    assert(set_var_declaration(&global_sym_table, &item1, true) == 2);
    //so var declarations cannot be set
    assert(get_var_declaration(&global_sym_table, &item1, &err) == false);
    //thus err flag is raised
    assert(err == true);

    //same case as shoown earlier, func can't be constant
    assert(set_constant(&global_sym_table, &item1, true) == 2);
    
    assert(get_constant(&global_sym_table, &item1, &err) == false);

    assert(err == true);


    symtab_t local_symtable;

    symtable_init(&local_symtable,11);

    assert(set_local_symtable(&global_sym_table, &item1, &local_symtable) == 0);
    
    assert(((symtab_t *)get_local_symtable(&global_sym_table, &item1,&err) )== &local_symtable);

    assert(err == false);


    dstring_free(&item1);
    dstring_free(&value1);
    
    symtable_dispose(&global_sym_table);
    symtable_dispose(&local_symtable);

    return 0;
}