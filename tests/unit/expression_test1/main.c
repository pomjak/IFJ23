#include <stdlib.h>
#include <stdio.h>
#include "expression.h"

int main(void)
{
    FILE *stdinput = freopen("input.txt", "r", stdin);

    int return_code = expr();

    fclose(stdinput);

    return return_code;
}