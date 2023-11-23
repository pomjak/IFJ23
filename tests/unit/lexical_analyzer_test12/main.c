/**
 * @file main.c
 * @author Jakub Pomsar (xpomsa00@stud.fit.vutbr.cz)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexical_analyzer.h"

int main(int argc, char ** argv) {
    
    token_T token;

    do {
        if (get_token(&token)) return 1;
        print_token(token);
    } while (token.type != TOKEN_EOF);
    
    return 0;
}
