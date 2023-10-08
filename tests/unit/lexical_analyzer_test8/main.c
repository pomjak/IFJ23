/**
 * @name IFJ23
 * @file main.c
 * @brief main to run lexical analyzer
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 08.10.2023
 **/

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
