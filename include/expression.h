/**
 * @file expression.h
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Expression parsing structures, precedence table and functions declarations
 * @date 2023-10-18
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "error.h"
#include "symstack.h"
#include "parser.h"

#define PREC_TABLE_SIZE 9
#define MAX_EXPR_SIZE 3

typedef enum PREC_TABLE_OPERATIONS
{
    S, // shift
    R, // reduce
    E, // equal
    X  // error
} prec_table_operation_t;

typedef enum PREC_TAB_INDEX
{
    INDEX_MUL_DIV = 0,
    INDEX_ADD_SUB,
    INDEX_NILL_CHECK,
    INDEX_IDENTIFIER,
    INDEX_RELATIONAL_OP,
    INDEX_LPAR,
    INDEX_RPAR,
    INDEX_NOT_NIL,
    INDEX_DOLLAR
} prec_tab_index_t;

// precedense rules
typedef enum PREC_RULES
{
    // arithmetic rules
    RULE_E_PLUS_E,  // E -> E + E
    RULE_E_MINUS_E, // E -> E - E
    RULE_E_MUL_E,   // E -> E * E
    RULE_E_DIV_E,   // E -> E / E

    // relational rules
    RULE_E_LT_E,     // E -> E < E
    RULE_E_LEQ_E,    // E -> E <= E
    RULE_E_GT_E,     // E -> E > E
    RULE_E_GEQ_E,    // E -> E >= E
    RULE_E_EQ_E,     // E -> E == E
    RULE_E_NEQ_E,    // E -> E != E
    RULE_E_IS_NIL_E, // E -> E ?? E
    RULE_E_NOT_NIL,  // E -> E!

    RULE_PARL_E_PARR, // E -> (E)
    RULE_OPERAND,     // E -> id
    RULE_NO_RULE
} prec_rule_t;

/***********************
 * Dynamic symbol array *
 ***********************/

typedef struct SYMBOL_ARR
{
    symstack_data_t *arr;
    size_t size;
} symbol_arr_t;

/**
 * @brief initializes dynamic symbol array
 */
void symbol_arr_init(symbol_arr_t *new_arr);

/**
 * @brief add node to symbol array
 *
 * @param node
 * @return true - added succefully
 * @return false - node was not added to array
 */
bool symbol_arr_append(symbol_arr_t *sym_arr, symstack_data_t data);

/**
 * @brief copies symbols to array from stack until it finds handle.
 *
 * @param stack
 * @param sym_arr
 */
void symbol_arr_copy_exp_to_arr(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief moves symbols to array from stack until it finds handle. Removes handle
 *
 * @param stack
 * @param sym_arr
 */
void symbol_arr_move_expr_to_arr(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief reverses symbol array
 *
 * @param sym_arr
 */
void symbol_arr_reverse(symbol_arr_t *sym_arr);

/**
 * @brief frees symbol arr
 *
 * @param sym_arr
 */
void symbol_arr_free(symbol_arr_t *sym_arr);

/**
 * @brief prints symbol array
 *
 */
void print_symbol_arr(symbol_arr_t *sym_arr);

/***********************************
 * Expression supportive functions *
 **********************************/

/**
 * @brief Sets first found error code, else nothing
 *
 * @return new error code
 */
int error_code_handler(int error_code);

/**
 * @brief pushes dollar on stack
 *
 * @param stack
 */
void push_initial_sym(symstack_t *stack);

/**
 * @brief checks if symbol is operand type (token type of INT STRING DOUBLE IDENTIFIER)
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_operand(symstack_data_t symbol);

/**
 * @brief checks if symbol is operator
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_binary_operator(symstack_data_t symbol);

/**
 * @brief Finds the closest token with preceding_eol flag set
 *
 * @param stack
 * @return true - end of line was found
 * @return false - end of line not found
 */
bool find_closest_eol(symstack_t *stack);

/**
 * @brief Deletes stack tokens until token with preceding_el flag set
 *
 * @param p - pointer to parser state
 * @param stack
 */
void reduce_to_eol(symstack_t *stack, Parser *p);

/**
 * @brief Finds if id is defined
 *
 * @param token
 * @param p - pointer to parser state
 * @return true - identifier is defined
 * @return false - identifier is not defined, or token is not identifier
 */
bool id_is_defined(token_T token, Parser *p);

/**
 * @brief Chekcs if symbol is literal
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_literal(symstack_data_t symbol);

/**
 * @brief checks if symbol is identifer
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_identifier(symstack_data_t symbol);

/**
 * @brief convert token to precedence table index based on it's data
 *
 * @param token
 * @return prec_tab_index_t - index
 */
prec_tab_index_t convert_token_to_index(token_T token);

/**
 * @brief converts term to precedence table index based on it's data
 *
 * @param data
 * @return prec_tab_index_t - index
 */
prec_tab_index_t convert_term_to_index(symstack_data_t data);

/**
 * @brief Finds and returns the closest terminal from top of the stack
 *
 * @param stack
 * @return node_t* - if null, no treminal was found
 */
node_t *get_closest_terminal(symstack_t *stack);

/**
 * @brief Get the prec table operation object
 *
 * @param stack
 * @param token
 * @return prec_table_operation_t
 */
prec_table_operation_t get_prec_table_operation(symstack_t *stack, token_T token);

/**
 * @brief pushes symbol on stack
 *
 * @param stack
 * @param token
 */
void equal_shift(symstack_t *stack, token_T *token);

/**
 * @brief operation shift - sets up handle and pushes symbol on stack
 *
 * @param stack
 * @param token
 */
void shift(symstack_t *stack, token_T *token);

/**
 * @brief choses rule based on given operator
 *
 * @param data
 */
prec_rule_t choose_operator_rule(symstack_data_t data);

/**
 * @brief Get the rule based of symbols on stack
 *
 * @param p - pointer to parser data
 * @return prec_rule_t
 */
prec_rule_t get_rule(symbol_arr_t *sym_arr);

/**
 * @brief sets the isTerminal to false of chosen terminal
 *
 * @param term
 */
void push_non_term_on_stack(symstack_t *stack, symstack_data_t *term);

/**
 * @brief pushes result symbol of reduced expression
 *
 * @param stack
 */
void push_reduced_symbol_on_stack(symstack_t *stack, symbol_arr_t *sym_arr, prec_rule_t rule, Parser *p);

/**
 * @brief reduce symbols on stack
 *
 * @param p - pointer to parser data
 * @param sym_arr
 */
void reduce(symstack_t *stack, Parser *p);

/**
 * @brief report err in expression
 *
 * @param sym_arr
 */
void reduce_error(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief manages expression error state
 *
 * @param stack
 */
void expr_error(symstack_t *stack);

/**
 * @brief converts exrpession type to parser expr type
 *
 * @param type
 * @return Type
 */
Type convert_to_expr_type(token_type_T type);

/**
 * @brief processes expression
 *
 * @return int error code
 */
int expr(Parser *parser_data);

/****************************
 * Reduction rule functions *
 ****************************/

/**
 * @brief reduces operand; in case of variable checks if it is defined. Generates code for operand push on data stack
 * 
 * @param operand symbol from stack
 * @param p parser state
 * @return symstack_data_t; reduced symbol to push on stack
 */
symstack_data_t process_operand(symstack_data_t *operand, Parser *p);

/**
 * @brief reduces arithmetic operations or redirect to other operation. Checks arithmetic operation semantics. Generates code 
 * for appropriate operation
 * 
 * @param sym_arr array of symbols to be reduced
 * @return symstack_data_t; reduced symbol to push on stack
 */
symstack_data_t process_arithmetic_operation(symbol_arr_t *sym_arr);

/**
 * @brief reduces divison and checks division semantics and generate code for division
 * 
 * @param first_operand 
 * @param second_operand 
 * @return symstack_data_t; reduced symbol to push on stack
 */
symstack_data_t process_division(symstack_data_t * first_operand, symstack_data_t * second_operand);

/**
 * @brief reduces concatenation and checks concatenation semantics. Generate code for concatenation
 * 
 * @param sym_arr 
 * @return symstack_data_t 
 */
symstack_data_t process_concatenation(symbol_arr_t *sym_arr);

/**
 * @brief reduces relational operation and checks relational operations semantics. Generate code for appropriate relational operation
 * 
 * @param sym_arr 
 * @param p 
 * @return symstack_data_t 
 */
symstack_data_t process_relational_operation(symbol_arr_t *sym_arr, Parser *p);

/**
 * @brief reduces expression in parenthesis.
 * 
 * @param sym_arr 
 * @return symstack_data_t 
 */
symstack_data_t process_parenthesis(symbol_arr_t *sym_arr);

/**************************
 * Type related functions *
 **************************/

/**
 * @brief strictly compares type of given operands. Compares core types and nilability
 * 
 * @param operand1 
 * @param operand2 
 * @return true operands are equal
 * @return false operands are not equal
 */
bool compare_types_strict(symstack_data_t *operand1, symstack_data_t *operand2);

/**
 * @brief compares operand core type with given type
 * 
 * @param operand 
 * @param type 
 * @return true operand has given type
 * @return false operand does not have given type
 */
bool compare_operand_with_type(symstack_data_t *operand, Type type);

/**
 * @brief returns nilability of given operand
 * 
 * @param operand 
 * @return true operand is nilable
 * @return false operand is not nilable
 */
bool operand_is_nilable(symstack_data_t *operand);

/**
 * @brief function for type control in assignment. Implicit reptying if possible
 * 
 * @param final_expr final expression type 
 * @param p parser state
 */
void verify_lhs_type(symstack_data_t *final_expr, Parser *p);

/**
 * @brief performs implicit conversion of operands to have same types if allowed.
 * 
 * @param operand1 
 * @param operand2 
 */
void convert_if_retypeable(symstack_data_t *operand1, symstack_data_t *operand2);

#endif
