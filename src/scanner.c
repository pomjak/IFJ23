#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "error.h"

/*************************************
 * State functions declarations      *
**************************************/
scanner_state_t start_get_state(int c);


/*************************************
 * Debug functions declarations      *
**************************************/
void print_token_type(token_t token);


#ifdef SCANNER_TEST
int main(int argc, char ** argv)
{
    (void) argc;
    (void) argv;
    print_error(ERR_INTERNAL, "This is a debugging test");
    return EXIT_SUCCESS;
}
#endif

/**
 * @brief Get the token object
 *
 * @param token
 * @return int return code
 */
int get_token(token_t * token)
{
    token->type = SCANNER_TOKEN_UNDEFINED;

    scanner_state_t state = SCANNER_STATE_START;
    char c =  fgetc(stdin);
    while(c != EOF)
    {
        switch (state)
        {
        case SCANNER_STATE_START:
            state = start_get_state(c);
            break;
        
        case SCANNER_STATE_EOF:
            token->type = SCANNER_TOKEN_EOF;
            return EXIT_SUCCESS;

        case SCANNER_STATE_NUMBER:
        //
            token->type = SCANNER_TOKEN_DOUBLE;
            return EXIT_SUCCESS;
            
        default:
            return ERR_LEXICAL;
        }
    }

    return EXIT_SUCCESS;
}


/************************
 * State functions      *
************************/

scanner_state_t start_get_state(int c)
{

    if(isdigit(c)) return SCANNER_STATE_NUMBER;
    else if(c == EOF) return SCANNER_STATE_EOF;
    
    return SCANNER_STATE_ERROR;
}



/************************
 * Debuggig functions   *
************************/

/**
 * @brief prints token type
 */
void print_token_type(token_t token)
{
    char type[20];
    switch (token.type)
    {
    case SCANNER_TOKEN_INT:
        strcpy(type,"INT");
        break;
    case SCANNER_TOKEN_DOUBLE:
        strcpy(type,"FLOAT");
        break;
    case SCANNER_TOKEN_STRING:
        strcpy(type,"STRING");
        break;
    default:
        printf("Unknown token type.\n");
        return;
    }
    printf("%s\n",type);
}
