/**
 * @file main.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief   test 01 token buffer
 * @date 2023-11-15
 *
 */

#include "token_buffer.h"
#include "lexical_analyzer.h"
#include "error.h"
#include <assert.h>

int main()
{
    token_buffer_t buffer;
    tb_init(&buffer);
    token_T token;
    token_T tmp;
    int result;
    do {
        if (get_token(&token)) {
            fprintf(stderr, "get_token err");
            tb_dispose(&buffer);
            return 1;
        }
        tb_push(&buffer, token);
        DEBUG_PRINT("pushed token: %d", token.type);
    } while (token.type != TOKEN_EOF);

    buffer.runner = buffer.head;
    DEBUG_PRINT("reset runner");

    if(!tb_peek(&buffer)) {
        ERROR_PRINT("head empty");
        return 1;
    }

    tmp = tb_get_token(&buffer);
        DEBUG_PRINT("tb_get_token: %d", tmp.type);
        if(tmp.type == TOKEN_UNDEFINED) {
            ERROR_PRINT("token undef");
            return 1;
        }
        print_token(tmp);

    do {
        tb_next(&buffer);
        tmp = tb_get_token(&buffer);
        DEBUG_PRINT("tb_get_token: %d", tmp.type);
        if(tmp.type == TOKEN_UNDEFINED) {
            ERROR_PRINT("token undef");
            return 1;
        }
        print_token(tmp);

    } while (tmp.type != TOKEN_EOF);
    
    // tb_pop(&buffer);
    buffer.runner = buffer.head;


    tmp = tb_get_token(&buffer);
        DEBUG_PRINT("tb_get_token: %d", tmp.type);
        if(tmp.type == TOKEN_UNDEFINED) {
            ERROR_PRINT("token undef");
            return 1;
        }
        print_token(tmp);
    do {
        tb_pop(&buffer);
        tmp = tb_get_token(&buffer);
        DEBUG_PRINT("tb_get_token: %d", tmp.type);
        if(tmp.type == TOKEN_UNDEFINED) {
            ERROR_PRINT("token undef");
            return 1;
        }
        print_token(tmp);

    } while (tmp.type != TOKEN_EOF);
    // tb_dispose(&buffer);
    return 0;
}