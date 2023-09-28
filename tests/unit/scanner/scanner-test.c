#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../../../src/scanner.h"
#include "../../../src/error.h"
#include "/Library/Frameworks/Unity/src/unity.h"

/**
 * ALERT: dont delete setUp and tearDown functions - they are declared due to fix compilation error
 */
void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

FILE * open_test_file(const char * file)
{
    printf("%s\n",file);
    FILE * fp = fopen(file, "r");
    if(fp == NULL)
    {
        print_error(ERR_INTERNAL,"Failed to open \"%s\" file.\n",file);
    }
    fp = freopen(file, "r",stdin);
    if(fp == NULL)
    {
        print_error(ERR_INTERNAL,"Failed to reopen \"%s\" file.\n",file);
    }
    return fp;
}

void double_correct_format(void)
{
    FILE * fp = open_test_file("test-inputs/double.in");

    // intialize token
    token_t token;
    token.type = SCANNER_TOKEN_UNDEFINED;

    // itereate through all tokens
    while(token.type != SCANNER_TOKEN_EOF)
    {
        get_token(&token);
        TEST_ASSERT(token.type == SCANNER_TOKEN_DOUBLE);
    }
    
    fclose(fp);
}

void double_wrong_format(void)
{
    FILE * fp = open_test_file("test-inputs/double-wrong-formats.in");
    token_t token;
    get_token(&token);
    fclose(fp);
}

void string_test(void)
{
    TEST_ASSERT_EQUAL_STRING("test","test");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(double_correct_format);
    return UNITY_END();
}
