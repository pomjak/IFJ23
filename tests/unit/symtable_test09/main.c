/**
 * @file main.c (unit test 9)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 9 - operations with multiple params [1021]
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item, param;
    unsigned int error;

    FILE *input = fopen("input.txt", "r");

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);
    dstring_init(&item);

    dstring_init(&param);

    dstring_add_const_str(&item, "func_with_lots_of_params");

    symtable_insert(&global_sym_table, &item, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item, function, &error);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item, &error) == function);
assert(error == SYMTAB_OK);

    char buffer[50];

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&param);

        dstring_add_const_str(&param, buffer);

        add_param(&global_sym_table, &item, &param, &error);
        assert(error == SYMTAB_OK);

        set_param_type(&global_sym_table, &item, &param, i % 6, &error);
        assert(error == SYMTAB_OK);
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&param);

        dstring_add_const_str(&param, buffer);

        symtab_item_t *get = (symtable_search(&global_sym_table, &item,&error));
        assert(error == SYMTAB_OK);

        assert(search_param(get->parameters, &param,&error) != NULL);
        assert(error == SYMTAB_OK);

        assert(get_param_type(&global_sym_table, &item, &param,&error) == i % 6);
        assert(error == SYMTAB_OK);

    }

    fclose(input);

    dstring_free(&item);

    dstring_free(&param);

    symtable_dispose(&global_sym_table);

    return 0;
}