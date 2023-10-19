/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 10 - used every created function
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
    dstring_t item1, item2, item3, item4, param, label;
    bool err_bl;
    unsigned int err_int;

    /**
     * @brief init of dstrings
     *
     */
    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");
    dstring_init(&item2);
    dstring_add_const_str(&item2, "item2");
    dstring_init(&item3);
    dstring_add_const_str(&item3, "item3");
    dstring_init(&item4);
    dstring_add_const_str(&item4, "item4");
    dstring_init(&param);
    dstring_add_const_str(&item4, "param");
    dstring_init(&label);
    dstring_add_const_str(&item4, "label");
    symtable_init(&global_sym_table);

    /**
     * @brief insert items into symtab
     *
     */
    assert(symtable_insert(&global_sym_table, &item1) == 0);
    assert(symtable_insert(&global_sym_table, &item2) == 0);
    assert(symtable_insert(&global_sym_table, &item3) == 0);
    assert(symtable_insert(&global_sym_table, &item4) == 0);

    symtable_clear(&global_sym_table);

    /**
     * @brief check if they are actually stored in symtab
     *
     */
    assert(is_in_symtable(&global_sym_table, &item1) == false);
    assert(is_in_symtable(&global_sym_table, &item2) == false);
    assert(is_in_symtable(&global_sym_table, &item3) == false);
    assert(is_in_symtable(&global_sym_table, &item4) == false);

    symtable_dispose(&global_sym_table);
    
    dstring_free(&item1);
    dstring_free(&item2);
    dstring_free(&item3);
    dstring_free(&item4);
    dstring_free(&param);
    dstring_free(&label);

    return 0;
}