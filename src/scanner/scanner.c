#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

FILE * source_file;

bool set_source_file(char * file);
void print_token_type(token_t token);
int get_token(token_t * token);

// process_number(char * num);
// process_keyword();
// process_string();

#ifdef SCANNER_TEST
int main(int argc, char ** argv)
{
    if(argc <= 1 || argc > 2) return 10;
    if(set_source_file(argv[1]) == ) return EXIT_FAILURE;

    token_t token;
    while ( get_token(&token) != EOF)
    {
        print_token_type(token);
    }
    

    return EXIT_SUCCESS;
}
#endif

/**
 * @brief Set the source file 
 * 
 * @param file name of file
 * @return true if success 
 */
bool set_source_file(char * file)
{
    source_file = fopen(file,"r");
    if(source_file == NULL)
    {
        return false;
    }
    return true;
}

/**
 * @brief Get the token object
 * 
 * @param token 
 * @return int return code
 */
int get_token(token_t * token)
{
    (void)token;
    scanner_state_t state = START;
    char c;
    while((c = fgetc(source_file)))
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

/**
 * @brief prints token type
 * 
 */
void print_token_type(token_t token)
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
