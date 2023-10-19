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
    FILE *input = fopen("input.txt", "r");

    symtable_init(&global_sym_table);

    dstring_init(&item);

    dstring_init(&param);

    dstring_add_const_str(&item, "func_with_lots_of_params");

    assert(symtable_insert(&global_sym_table, &item) == 0);

    assert(set_type(&global_sym_table, &item, function) == 0);

    bool err = true;
    unsigned int error_int = 255;

    assert(get_type(&global_sym_table, &item, &err) == function);

    assert(err == false);

    char buffer[50];

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&param);

        dstring_add_const_str(&param, buffer);

        assert(add_param(&global_sym_table, &item, &param, &err) == 0);

        assert(set_param_type(&global_sym_table, &item, &param, i % 6) == 0);
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        err = true;

        dstring_clear(&param);

        dstring_add_const_str(&param, buffer);

        symtab_item_t *get = (symtable_search(&global_sym_table, &item));

        assert(search_param(get->parameters, &param) != NULL);

        assert(get_param_type(&global_sym_table, &item, &param, &error_int) == i % 6);

        assert(error_int == 0);
    }


    fclose(input);

    dstring_free(&item);

    dstring_free(&param);

    symtable_dispose(&global_sym_table);

    return 0;
}