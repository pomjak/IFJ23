#include <stdio.h>
#include <stdlib.h>
#include "symstack.h"

int main(void)
{
    symstack_t stack;
    init_stack(&stack);

    // push node
    data_t data;
    data.c = 'A';
    data.id = 1;
    push(&stack, data);
    push(&stack, data);

    print_stack(&stack, 10);

    printf("\n");
    print_line(10);
    printf("\n");

    data.c = 'X';
    data.id = 2;
    push(&stack, data);

    print_stack(&stack, 10);

    printf("\n");
    print_line(10);
    printf("\n");

    // remove node
    pop(&stack);
    pop(&stack);
    pop(&stack);
    print_stack(&stack, 10);
    return EXIT_SUCCESS;
}
