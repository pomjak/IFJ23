/**
 * @name IFJ23
 * @file main.c
 * @brief main to run lexical analyzer
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 07.10.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "lexical_analyzer.h"
#include "dyn_string.h"

int main(int argc, char ** argv) {
    
    token_T token;

    do {
        if (get_token(&token)) return 1;
        print_token(token);

        /**
         * Free string if token contains string 
         */
        if (token.type == TOKEN_STRING || token.type == TOKEN_IDENTIFIER) {
            dstring_free(&token.value.string_val);
        }
    } while (token.type != TOKEN_EOF);
    
    return 0;
}
