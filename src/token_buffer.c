/**
 * @file token_buffer.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief Linked list buffer for loading tokens
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "token_buffer.h"

void token_buffer_init(token_buffer_t* head) {
    *head = NULL;
}

void token_buffer_add(token_buffer_t* head, token_T token) {

}

void token_buffer_pop(token_buffer_t* head) {

}

void token_buffer_dispose(token_buffer_t* head) {
    
}

bool token_buffer_peek(token_buffer_t head) {
    return (head != NULL);
}