/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 12.11.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"

int main(int argc, char ** argv) {

    code_generator_prolog();
    
    code_generator_if_header(TOKEN_GT,  0);
    code_generator_if_header(TOKEN_GEQ, 0);
    code_generator_if_header(TOKEN_LT,  0);
    code_generator_if_header(TOKEN_LEQ, 0);
    code_generator_if_header(TOKEN_EQ,  0);
    code_generator_if_header(TOKEN_NEQ, 0);

    //

    code_generator_if_else(0);
    code_generator_if_end(0);
    
    return 0;
}
