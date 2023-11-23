#include <stdio.h>
#include <assert.h>
#include "parser.h"
#include "expression.h"

int main()
{
    Parser p;
    uint32_t res = 0;

    if (!parser_init(&p))
    {
        fprintf(stderr, "[ERROR %d] Initializing parser data failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }
    if (!add_builtins(&p))
    {
        parser_dispose(&p);
        fprintf(stderr, "[ERROR %d] adding builtin functions failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }
    if ((res = parser_fill_buffer(&p)))
    {
        parser_dispose(&p);
        fprintf(stderr, "[ERROR %d] loading tokens to buffer failed\n", res);
        return res;
    }

    p.buffer.runner = p.buffer.head;
    p.curr_tok = tb_get_token(&p.buffer);
    DEBUG_PRINT("first token loaded : %d", p.curr_tok.type);
    if ((p.curr_tok.type == TOKEN_UNDEFINED) && (p.curr_tok.value.int_val == 0))
    {
        fprintf(stderr, "[ERROR %d] Getting first token from buffer failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }

    int expr_res = expr(&p);

    assert(p.expr_res.expr_type == bool_);
    fprintf(stderr, "res = %d", expr_res);
    parser_dispose(&p);
    return expr_res;
}