#include <stdlib.h>
#include <stdio.h>
#include "expression.h"
#include "parser.h"
#include "symstack.h"

int main(void)
{
    FILE *stdinput = freopen("input.txt", "r", stdin);
    Parser parser;
    int return_code = expr(&parser);

    printf("Return code: %d\n", return_code);
    delete_token(&parser.curr_tok);
    fclose(stdinput);

    return 0;
}
