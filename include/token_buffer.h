/**
 * @file token_buffer.h
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief Linked list token buffer header file
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H

#include "lexical_analyzer.h"
#include "debug.h"

typedef struct token_buffer_node {
    token_T token;
    struct token_buffer_node* next;
    struct token_buffer_node* prev;
} *token_buffer_node_t;

typedef struct {
    token_buffer_node_t head;
    token_buffer_node_t runner;
} token_buffer_t;

/**
 * @brief Initialize the token buffer linked list
 * @param head Top of the list
 */
void tb_init(token_buffer_t* buffer);

/**
 * @brief Add a new token to the end of the buffer
 *
 * @param head Top of the list
 * @param token Added token
 * @return 0 on success
 * @return 1 on failure
 */
int tb_push(token_buffer_t* buffer, token_T token);

/**
 * @brief Remove last element from buffer
 *
 * @param head Top of the list
 */
void tb_pop(token_buffer_t* buffer);

/**
 * @brief Dispose of the contents in buffer
 *
 * @param head Top of the list
 */
void tb_dispose(token_buffer_t* buffer);

/**
 * @brief Check if buffer is safe to use (not empty)
 *
 * @param head Top of the list
 * @return true if not empty
 * @return false if empty
 */
bool tb_peek(token_buffer_t* buffer);

/**
 * @brief Get the current token from buffer and move to next item in the list
 *
 * @param head Current list element
 * @return token_T token \n token.type = TOKEN_UNDEFINED on error
 */
token_T tb_get_token(token_buffer_t* buffer);

void tb_next(token_buffer_t* buffer);
void tb_prev(token_buffer_t* buffer);

void tb_print_token_type(token_buffer_node_t head);

#endif