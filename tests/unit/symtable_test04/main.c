/**
 * @file main.c (unit test 4)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 4 - operations with multiple items [1021]
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item, value1;
    unsigned int error;

    FILE *input = fopen("input.txt", "r");

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    dstring_init(&item);

    char buffer[50];

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        symtable_insert(&global_sym_table, &item, &error);
        assert(error == SYMTAB_OK);

        set_type(&global_sym_table, &item, i % 6, &error);
        assert(error == SYMTAB_OK);
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        assert(symtable_search(&global_sym_table, &item, &error) != NULL);
        assert(error == SYMTAB_OK);

        assert(get_type(&global_sym_table, &item, &error) == i % 6);
        assert(error == SYMTAB_OK);
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        if (i % 2)
        {
            symtable_delete(&global_sym_table, &item, &error);
            assert(error == SYMTAB_OK);
        }
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        if (i % 2)
        {
            assert(symtable_search(&global_sym_table, &item, &error) == NULL);
            assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);
        }
        else
        {
            assert(symtable_search(&global_sym_table, &item, &error) != NULL);
            assert(error == SYMTAB_OK);
        }
    }

    fclose(input);

    dstring_free(&item);

    symtable_dispose(&global_sym_table);

    return 0;
}