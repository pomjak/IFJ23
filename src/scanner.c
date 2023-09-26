#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"


/**
 * TODO:
 * 1) restructure project according to assignment
 * 2) create FSM
 * 3) implement core states ( operators, keywords, strings)
 */

#ifdef SCANNER_TEST
int main(int argc, char ** argv)
{
    if(argc <= 1 || argc > 2) return 10;

    token_t token;
    while ( get_token(&token) != EOF)
    {
        print_token_type(token);
    }
    

    return EXIT_SUCCESS;
}
#endif

/************************
 * State functions      *
************************/


/**
 * @brief Get the token object
 * 
 * @param token 
 * @return int return code
 */
int getToken(token_t * token)
{
    (void)token;

    scanner_state_t state = START;
    char c;
    while((c = fgetc(stdin)))
    {
        switch (state)
        {
        case START:
            break;
        
        default:
            break;
        }
    }
    return EXIT_SUCCESS;
}


/************************
 * Debuggig functions   *
************************/

/**
 * @brief prints token type
 */
void prinTokenType(token_t token)
{
    char type[20];
    switch (token.type)
    {
    case INT:
        strcpy(type,"INT");
        break;
    case FLOAT:
        strcpy(type,"FLOAT");
        break;
    case STRING:
        strcpy(type,"STRING");
        break;
    
    default:
        printf("Unknown token type.\n");
        return;
    }
    printf("%s\n",type);
}
