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

int tb_push(token_buffer_node_t* head, token_T token) {

    token_buffer_node_t new_node = (token_buffer_node_t)malloc(sizeof(struct token_buffer_node));
    if (new_node == NULL) {
        return EXIT_FAILURE;
    }
    new_node->token = token;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    }
    else {
        token_buffer_node_t current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    return EXIT_SUCCESS;
}

void tb_pop(token_buffer_node_t* head) {

    if (tb_peek(*head)) {
        token_buffer_node_t current = *head;
        *head = current->next;
        if (current->token.type == TOKEN_STRING || current->token.type == TOKEN_IDENTIFIER)
            dstring_free(&current->token.value.string_val);
        free(current);
    }

}

void tb_dispose(token_buffer_t* buffer) {

    while (tb_peek(buffer->head)) {
        tb_pop(&buffer->head);
    }
}

bool tb_peek(token_buffer_node_t head) {
    return (head != NULL);
}

token_T tb_get_token(token_buffer_node_t* head) {

    if (!tb_peek(*head)) {
        WARNING_PRINT("empty buffer");
        token_T err_token;
        err_token.type = TOKEN_UNDEFINED;
        err_token.value.int_val = 0;
        err_token.preceding_eol = false;
        return err_token;
    }
    token_buffer_node_t current = *head;
    token_T curr_token = current->token;

    *head = current->next;
    DEBUG_PRINT("free(current)");
    free(current);


    return curr_token;
}

void tb_print_token_type(token_buffer_node_t head) {
    token_buffer_node_t current = head;

    while (current != NULL) {
        fprintf(stderr, "token: %d\n", current->token.type);
        current = current->next;
    }
}