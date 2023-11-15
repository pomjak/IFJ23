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
            tb_dispose(&buffer);
            return 1;
        }
        tb_push(&buffer, token);
        tmp = tb_get_token(&buffer);
        print_token(tmp);
        if(tmp.type == TOKEN_STRING || tmp.type == TOKEN_IDENTIFIER) {
            dstring_free(&tmp.value.string_val);
        }
    } while (token.type != TOKEN_EOF);

    tb_dispose(&buffer);

    return 0;
}