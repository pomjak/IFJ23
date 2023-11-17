/**
 * @file main.c
 * @author Jakub Pomsar (xpomsa00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2023-11-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <assert.h>
#include "parser.h"

int main()
{
    uint32_t res;
    res = parse();
    fprintf(stderr, "res: %d", res);
    return res;
}