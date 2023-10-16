/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 2 insert & search of one item [insert,search, set_type, get_type]
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

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");
    symtable_init(&global_sym_table);

    assert(symtable_search(&global_sym_table, &item1) == NULL);

    symtable_insert(&global_sym_table, &item1);
    
    bool err;

    assert(set_type(&global_sym_table, &item1, function) == 0);

    assert(get_type(&global_sym_table, &item1, &err) == function);
    
    assert(err == false);

    dstring_free(&item1);
    symtable_dispose(&global_sym_table);

    return 0;
}