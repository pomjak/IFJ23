/**
 * @file main.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief   test 01 token buffer
 * @date 2023-11-15
 *
 */

#include "token_buffer.h"
#include <assert.h>

int main()
{
    token_buffer_t buffer;
    token_buffer_init(&buffer);
    token_T token;
    int result;
    result = get_token(&token);
    assert(result == 0);
    assert(token_buffer_push(&buffer, token) == 0);
    token_buffer_dispose(&buffer);
    
    return 0;
}