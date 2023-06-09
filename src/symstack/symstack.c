#include <stdio.h>
#include <stdlib.h>

void hello_stack()
{
    printf("if you see this, you haven't implemented stack...\n");
}


#ifdef SYMSTACK_TEST

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;

    return EXIT_SUCCESS;
}

#endif
