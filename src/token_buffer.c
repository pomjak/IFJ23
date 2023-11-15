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

void tb_init(token_buffer_t* head) {
    *head = NULL;
}

int tb_push(token_buffer_t* head, token_T token) {

    token_buffer_t new_node = (token_buffer_t)malloc(sizeof(struct token_buffer_node));
    if (new_node == NULL) {
        return EXIT_FAILURE;
    }
    new_node->token = token;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    }
    else {
        token_buffer_t current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    return EXIT_SUCCESS;
}

void tb_pop(token_buffer_t* head) {

    if (tb_peek(*head)) {
        token_buffer_t current = *head;
        *head = current->next;
        free(current);
    }

}

void token_buffer_dispose(token_buffer_t* head) {

    while (tb_peek(*head)) {
        tb_pop(head);
    }
}

bool tb_peek(token_buffer_t head) {
    return (head != NULL);
}