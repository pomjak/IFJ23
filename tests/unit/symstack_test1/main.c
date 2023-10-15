#include <stdio.h>
#include <stdlib.h>
#include "symstack.h"

#define DISPLAY_WIDTH 10

int main(void)
{
    symstack_t stack;
    init_symstack(&stack);

    // push node
    data_t data;
    data.c = 'A';
    data.id = 1;
    symstack_push(&stack, data);
    symstack_push(&stack, data);

    print_stack(&stack, DISPLAY_WIDTH);

    printf("\n");
    print_line(DISPLAY_WIDTH);
    printf("\n");

    data.c = 'X';
    data.id = 2;
    symstack_push(&stack, data);

    print_stack(&stack, DISPLAY_WIDTH);

    printf("\n");
    print_line(DISPLAY_WIDTH);
    printf("\n");

    // remove node
    symstack_dispose(&stack);
    print_stack(&stack, DISPLAY_WIDTH);
    return EXIT_SUCCESS;
}
