#include <stdio.h>
#include <stdlib.h>

#ifdef PARSER_TEST

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;
    printf("parser was compiled! (only parser..)\n");

    return EXIT_SUCCESS;
}

#endif

