/**
 * @file symstack.h
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Stack for symbols used in precedence analysis
 * @date 2023-10-15
 */

#ifndef SYMSTACK_H
#define SYMSTACK_H
#include <stdbool.h>
#include <stdlib.h>
#include "lexical_analyzer.h"
#include "symtable.h"

#define MAX_TOKEN_SYMBOL_SIZE 4
typedef struct DATA
{
    token_T token;
    bool isTerminal;
    bool isHandleBegin;
    Type expr_type;
    char symbol[MAX_TOKEN_SYMBOL_SIZE];
} symstack_data_t;
typedef struct NODE
{
    symstack_data_t data;
    struct NODE *previous;
} node_t;

typedef struct SYMSTACK
{
    node_t *top;
    size_t size;
} symstack_t;

// /**
//  * @brief safely deletes token from memory
//  *
//  * @param token - token to be deleted
//  */
// void delete_token(token_T *token);

/**
 * @brief converts token to data
 *
 * @param token - tokeen to be ocnverted
 */
symstack_data_t convert_token_to_data(token_T token);

/**
 * @brief initialize stack
 *
 * @param stack - stack to be initilized
 */
void init_symstack(symstack_t *stack);

/**
 * @brief pushes concrete data on top of the stack
 *
 * @param stack - used stack
 * @param data - data to push
 * @return true - symbol pushed on stack
 * @return false - failed tu push symbol on stack
 */
bool symstack_push(symstack_t *stack, symstack_data_t data);

/**
 * @brief removes node from stack and return it's data
 *
 * @param stack
 * @return symstack_data_t - poped data from given stack
 */
symstack_data_t symstack_pop(symstack_t *stack);

/**
 * @brief returns data from top of the stack, but does not remove the node
 *
 * @param stack
 * @return symstack_data_t - returned pointer to top of the stack
 */
node_t *symstack_peek(const symstack_t *stack);

/**
 * @brief checks if the given stack is empty
 *
 * @param stack
 * @return true - if stack is empty
 * @return false - stack is not empty
 */
bool symstack_is_empty(const symstack_t *stack);

/**
 * @brief delete all nodes from stack
 *
 * @param stack
 * @return true - deleted all data
 * @return false - not deleted all data
 */
bool symstack_dispose(symstack_t *stack);

/**
 * @brief get token and  based of the token type returns readable string
 *
 * @param token
 * @return const char*
 */
const char *convert_token_type_to_string(token_T token);

/**
 * @brief prints stack line
 *
 * @param width
 */
void print_line(unsigned int width);

/**
 * @brief prints node of the stack
 *
 * @param node
 * @param width
 */
void print_node(node_t *node, unsigned int width);

/**
 * @brief prints whole stack
 *
 * @param stack
 * @param width
 */
void print_stack(const symstack_t *stack, unsigned int width);
#endif
