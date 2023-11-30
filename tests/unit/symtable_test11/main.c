/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 11 - uid 
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include "scope.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    scope_t stack;
    dstring_t item1, item2, item3, item4, item5, item6;
    unsigned int error;

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

    dstring_init(&item5);
    dstring_add_const_str(&item5, "param");

    dstring_init(&item6);
    dstring_add_const_str(&item6, "label");

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    init_scope(&stack);
    add_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief insert items into symtab
     *
     */
    symtable_insert(&global_sym_table, &item1, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(&global_sym_table, &item2, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item3, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item4, &error);
    assert(error == SYMTAB_OK);

    add_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item5, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item6, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief asserting correct uid was stored
     *
     */

    assert(symtable_search(&global_sym_table, &item1, &error)->uid == 0);
    assert(error == SYMTAB_OK);

    assert(symtable_search(&global_sym_table, &item2, &error)->uid == 1);
    assert(error == SYMTAB_OK);

    assert(search_scopes(stack, &item3, &error)->uid == 2);
    assert(error == SYMTAB_OK);

    assert(search_scopes(stack, &item4, &error)->uid == 3);
    assert(error == SYMTAB_OK);

    assert(search_scopes(stack, &item5, &error)->uid == 4);
    assert(error == SYMTAB_OK);

    assert(search_scopes(stack, &item6, &error)->uid == 5);
    assert(error == SYMTAB_OK);

    /**
     * @brief dispose whole symtable and dstrings
     *
     */
    dispose_scope(&stack, &error);
    symtable_dispose(&global_sym_table);

    dstring_free(&item1);
    dstring_free(&item2);
    dstring_free(&item3);
    dstring_free(&item4);
    dstring_free(&item5);
    dstring_free(&item6);

    return 0;
}