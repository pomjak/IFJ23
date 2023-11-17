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
#include <stdio.h>

void tb_init(token_buffer_t* buffer) {
    buffer->head = NULL;
    buffer->runner = buffer->head;
}

int tb_push(token_buffer_t* buffer, token_T token) {

    token_buffer_node_t new_node = (token_buffer_node_t)malloc(sizeof(struct token_buffer_node));
    if (new_node == NULL) {
        return EXIT_FAILURE;
    }
    new_node->token = token;
    new_node->next = NULL;

    /* Add first element to an empty list */
    if (!tb_peek(buffer)) {
        buffer->head = new_node;
        new_node->prev = NULL;
        buffer->runner = buffer->head;
    }
    else {
        token_buffer_node_t current = buffer->runner;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
        new_node->prev = current;
    }

    return EXIT_SUCCESS;
}

void tb_pop(token_buffer_t* buffer) {

    if (tb_peek(buffer)) {
        token_buffer_node_t current = buffer->runner;
        if(current->prev != NULL) {
            current->prev->next = current->next;
        }
        if(current->next != NULL) {
            current->next->prev = current->prev;
        }
        if(buffer->runner == buffer->head) {
            buffer->head = current->next;
        }
        buffer->runner = current->next;
        if (current->token.type == TOKEN_STRING || current->token.type == TOKEN_IDENTIFIER)
            dstring_free(&current->token.value.string_val);
        free(current);
    }

}

void tb_dispose(token_buffer_t* buffer) {
    buffer->runner = buffer->head;
    while (tb_peek(buffer)) {
        tb_pop(buffer);
    }
    return;
}

bool tb_peek(token_buffer_t* buffer) {
    return (buffer->head != NULL);
}

token_T tb_get_token(token_buffer_t* buffer) {

    if (!tb_peek(buffer)) {
        WARNING_PRINT("empty buffer");
        token_T err_token;
        err_token.type = TOKEN_UNDEFINED;
        err_token.value.int_val = 0;
        err_token.preceding_eol = false;
        return err_token;
    }
    token_T curr_token = buffer->runner->token;

    return curr_token;
}

void tb_print_token_type(token_buffer_node_t head) {
    token_buffer_node_t current = head;

    while (current != NULL) {
        fprintf(stderr, "token: %d\n", current->token.type);
        current = current->next;
    }
}

void tb_next(token_buffer_t* buffer) {
    if(buffer->runner->next != NULL) {
        buffer->runner = buffer->runner->next;
    }
}
void tb_prev(token_buffer_t* buffer) {
    if(buffer->runner->prev != NULL) {
        buffer->runner = buffer->runner->prev;
    }
}