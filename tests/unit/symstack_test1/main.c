/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Testing ovrer all functionality - push, pop and dispose function
 * @date 2023-10-16
 */

#include <stdio.h>
#include <stdlib.h>
#include "symstack.h"

/**
 * TODO:
 * new test for get tokens too
 *
 */
#define DISPLAY_WIDTH 1

int main(void)
{
    symstack_t stack;
    init_symstack(&stack);

    token_T token;
    get_token(&token);
    while (token.type != TOKEN_EOF)
    {
        symstack_data_t data = convert_token_to_data(token);
        get_token(&token);
        symstack_push(&stack, data);
    }
    print_stack(&stack, DISPLAY_WIDTH);

    printf("\nsymstack_dispose:\n");
    symstack_dispose(&stack);
    print_stack(&stack, DISPLAY_WIDTH);
    if (!symstack_is_empty(&stack))
    {
        printf("Stack is not empty.\n");
    }
    else
    {
        printf("Stack is empty.\n");
    }
    return EXIT_SUCCESS;
}
