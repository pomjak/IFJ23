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

#include "scope.h"
#include <assert.h>

int main()
{
    scope_t stack;
    dstring_t item, value1;
    unsigned int error;

    FILE *input = fopen("input.txt", "r");

    init_scope(&stack);

    dstring_init(&item);

    char buffer[50];

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        add_scope(&stack, &error);
        assert(error == SYMTAB_OK);

        symtable_insert((*stack).local_sym, &item, &error);
        assert(error == SYMTAB_OK);

        search_scopes(stack, &item, &error)->type = i % 6;
        assert(error == SYMTAB_OK);
    }

    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        assert(search_scopes(stack, &item, &error) != NULL);
        assert(error == SYMTAB_OK);

        assert(search_scopes(stack, &item, &error)->type == i % 6);
        assert(error == SYMTAB_OK);
    }

    rewind(input);
    int i = 0;
    for (i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        if (i % 2)
        {
            pop_scope(&stack, &error);
            assert(error == SYMTAB_OK);
        }
    }
    int cnt = i;
    rewind(input);

    for (int i = 0; fgets(buffer, sizeof(buffer), input) != NULL; i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        if (i < (cnt / 2)+1)
        {
            assert(search_scopes(stack, &item, &error) != NULL);
            assert(error == SYMTAB_OK);
        }
        else
        {
            assert(search_scopes(stack, &item, &error) == NULL);
            assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);
        }
    }

    fclose(input);

    dstring_free(&item);

    dispose_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    return 0;
}