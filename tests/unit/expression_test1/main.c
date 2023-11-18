#include <stdlib.h>
#include <stdio.h>
#include "expression.h"

int main(void)
{
    FILE *stdinput = freopen("input.txt", "r", stdin);
    int return_code = expr();
    printf("Return code: %d\n", return_code);
    fclose(stdinput);

    return 0;
}
