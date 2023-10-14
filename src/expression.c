#include "error.h"
#include "expression.h"
#include "stack.h"
#include "scanner.h"
#include "symtable.h"
#include <stdbool.h>

#define CLEAN_BEFORE_ERROR_EXIT(_stack) \
do {\
    stack_dispose(_stack);\
} while(0)

#define PUSH_SYMBOL_ON_STACK(_stack,_symbol)\
do{\
    if(!push(&_stack,_symbol))\
    {\
        print_error(INTERNAL_ERROR,"expressions.c: Failed tu push symbol on stack.");\
        CLEAN_BEFORE_ERROR_EXIT(&_stack);\
        return INTERNAL_ERROR;\
    }\
}while(0);

#define GET_TOKEN_AND_REPAIR(_token) \
do{\
    if(get_token(_token) == TOKEN_ERR)\
    {\
        if(current_token->token_type == TOKEN_EQ) eq_or_neq.symbol = SYM_EQ;\
        else if(current_token->token_type == TOKEN_NEQ) eq_or_neq.symbol = SYM_NEQ;\
        PUSH_SYMBOL_ON_STACK(prec_stack,eq_or_neq);\
        func_return =  TOKEN_ERR;\
    }\
}while (0);


/**
 * HELPFUL FUNCTION MACROS FOR REDUCING 
 * @brief push non-term on stack with expression result type
 * 
 */
#define PUSH_NON_TERM_ON_STACK(_data,_stack,_result_type) \
    _data.data_type = _result_type;\
    _data.symbol = NON_TERMINAL;\
    push(_stack,_data);

#define PREC_TABLE_SIZE 8
typedef enum PREC_TABLE_OPERATIONS {
    S,  //shift
    R,  //reduce
    E,  //equal
    X   //error
}prec_table_operations_t;


//RO - relational operators , RE - relational equality operators 
prec_table_operations_t prec_tab[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = 
{
    /*       +-. | / * | RO | RE | id | $ | ( | ) | */
    /* +-. */{R,S,R,R,S,R,S,R},
    /* / * */{R,R,R,R,S,R,S,R},
    //
    /* RO  */{S,S,R,R,S,R,S,R},
    /* RE  */{S,S,S,R,S,R,S,R},
    /* id  */{R,R,R,R,X,R,X,R},
    //
    /* $   */{S,S,S,S,S,X,S,X}, //maybe konec X,X,X
    /* (   */{S,S,S,S,S,X,S,E},
    /* )   */{R,R,R,R,X,R,X,R}
};

static void print_stack(sym_stack_t * stack)
{
    printf("----------STACK-------------\n");
    // printf("Depth: %d\n",get_depth(stack));
    node * tmp = (*stack);
    char s[4] = { 0, 0, 0, 0};
    while (tmp != NULL)
    {
        switch (tmp->data.symbol)
        {
         case SYM_PLUS: // E -> E + E
            s[0] = '+';
            s[1] = 0;
            break;
        case SYM_MINUS: // E -> E - E
            s[0] = '-';
            s[1] = 0;
            break;
        case SYM_CONCAT: // E -> E.E
            s[0] = '.';
            s[1] = 0;
            break;
        case SYM_MUL: // E -> E * E
            s[0] = '*';
            s[1] = 0;
            break;
        case SYM_DIV: // E -> E / E
            s[0] = '/';
            s[1] = 0;
            break;

            //relational operator rules
        case SYM_LT: // E -> E < E
            s[0] = '>';
            s[1] = 0;
            break;
        case SYM_LEQ: // E -> E <= E
            s[0] = '<';
            s[1] = '=';
            s[2] = 0;
            break;
        case SYM_GT: // E -> E > E
            s[0] = '>';
            s[1] = 0;
            break;
        case SYM_GEQ: // E -> E >= E
            s[0] = '>';
            s[1] = '=';
            break;
        case SYM_EQ: // E -> E === E
            s[0] = '=';
            s[1] = '=';
            s[2] = '=';
            s[3] = 0;
            break;
        case SYM_NEQ:  // E -> E !== E
            s[0] = '!';
            s[1] = '=';
            s[2] = '=';
            s[3] = 0;
            break;
        case SYM_DOLLAR:
            s[0] = '$';
            s[1] = 0;
            break;
        case SYM_REDUCE_END:
            s[0] = 'R';
            s[1] = 'N';
            s[2] = 'D';
            s[3] = 0;
            break;
        case NON_TERMINAL:
            s[0] = 'E';
            break;
        case SYM_NULL:
        case SYM_FLOAT:
        case SYM_INT:
        case SYM_STRING:
        case SYM_ID:
            s[0] = 'I';
            s[1] = 'D';
            break;
        case SYM_PAR_L:
            s[0] = '(';
            break;
        case SYM_PAR_R:
            s[0] = ')';
            break;
        default:
            printf("\t\t\t\t\t\t\t\t\tUNDIFNED TOKEN: %d\n",tmp->data.symbol);
            s[0] = 'U';
            s[1] = 'N';
            break;
        }
        // printf("|%d|%d|%d|%d|\n",s[0],s[1],s[2],s[3]);
        printf("Sym on stack: %s\n",s);
        s[0] = 0;
        s[1] = 0;
        s[2] = 0;
        s[3] = 0;
        tmp = tmp->next;
    }
    printf("--------STACK-END-----------\n");
}

/**
 * @brief semathic check for arithmetic operation
 * 
 * @param operators 
 * @param rule 
 * @param error_info 
 * @return type_t 
 */
static type_t check_operator_types(stack_data_t * operators, prec_rule_t rule, error_t * error_info)
{
    type_t exp_type = UNDEFINED_T;
    if(rule == E_CONCAT_E)
    {
        if(operators[0].data_type == STRING_T && operators[2].data_type == STRING_T) exp_type = STRING_T;
        else
        {
            //add line info errors
            print_error(MISSTYPE_OPERATION_ERR,"Not strings concatenating.");
            *error_info = MISSTYPE_OPERATION_ERR;
            return exp_type; //maybe change
        }
    }
    else
    {  
        
        if(rule >= E_LT_E && rule <= E_NEQ_E) //relational operators
        {
            if(rule == E_EQ_E || rule == E_NEQ_E) //strict compare
            {
                if(operators[0].data_type != operators[2].data_type)
                {
                    // rule == E_EQ_E ? false : true; //you can print here - this comparison is always true/false based on operator
                }
            }
            else //<,>,<=,>=
            {
                //retyping
            }
        }
        else if(rule >= E_PLUS_E && rule <= E_DIV_E)//arithmetic operators check if the operators are numbers 
        {
            if((operators[0].data_type == FLOAT_T || operators[0].data_type == INT_T) && (operators[2].data_type == FLOAT_T || operators[2].data_type == INT_T))
            {
                //corection type
                if(operators[0].data_type == operators[2].data_type) exp_type = operators[0].data_type;
                else if(operators[0].data_type == FLOAT_T || operators[2].data_type == FLOAT_T) 
                {
                    if(operators[0].data_type == FLOAT_T)
                    {
                        //generate retype code of first operator
                    }
                    else
                    {
                        //generate retype code of second operator
                    }
                    exp_type = FLOAT_T;
                }
                else exp_type =  INT_T;
            }
            else *error_info = MISSTYPE_OPERATION_ERR;
        }
    }
    return exp_type;
}

/**
 * @brief Get the symbol from token object
 * 
 * @param token 
 * @return prec_table_idx_op_t symbol
 */
static prec_sym_type_t get_symbol_from_token(Token * token) 
{
    switch (token->token_type)
    {
    //arithmetic operators
    case TOKEN_ADD:
        return SYM_PLUS;
    case TOKEN_SUB:
        return SYM_MINUS;
    case TOKEN_DOT:
        return SYM_CONCAT;
    case TOKEN_MUL:
        return SYM_MUL;
    case TOKEN_KEYWORD:
        return SYM_NULL;
    case TOKEN_DIV:
        return SYM_DIV;

    //relational operators
    case TOKEN_LT:
        return SYM_LT;
    case TOKEN_LEQ:
        return SYM_LEQ;
    case TOKEN_GT:
        return SYM_GT;
    case TOKEN_GEQ:
        return SYM_GEQ;
    case TOKEN_EQ:
        return SYM_EQ;
    case TOKEN_NEQ:
        return SYM_NEQ;

    //parantheses
    case TOKEN_PAR_L:
        return SYM_PAR_L;
    case TOKEN_PAR_R:
        return SYM_PAR_R;

    //operands
    case TOKEN_IDENTIFIER:
        return SYM_ID;
    case TOKEN_STRING:
        return SYM_STRING;
    case TOKEN_INT:
        return SYM_INT;
    case TOKEN_FLOAT:
        return SYM_FLOAT;
    default:
        return SYM_DOLLAR;
    }
}

/**
 * @brief Get the prec table operator object
 * 
 * @param symbol 
 * @return prec_table_idx_op_t 
 */
static prec_table_idx_op_t get_prec_table_operator(prec_sym_type_t symbol) 
{
    switch (symbol)
    {
    // +-.
    case SYM_PLUS:
    case SYM_MINUS:
    case SYM_CONCAT:
        return IDX_ADD_SUB_CONCAT;
    // /*
    case SYM_MUL:
    case SYM_DIV:
        return IDX_MUL_DIV;
    // RO
    case SYM_LT:
    case SYM_LEQ:
    case SYM_GT:
    case SYM_GEQ:
        return IDX_REL_OP;
        
    // RE
    case SYM_EQ:
    case SYM_NEQ:
        return IDX_REL_EQ;
        
    // id
    case SYM_NULL:
    case SYM_ID:
    case SYM_INT:
    case SYM_FLOAT:
    case SYM_STRING:
        return IDX_ID;

    // (
    case SYM_PAR_L:
        return IDX_PAR_L;
    // )
    case SYM_PAR_R:
        return IDX_PAR_R;

    //$
    default:
        return IDX_DOLLAR_SYMBOL; 
    }
}

/**
 * @brief Get the number of operands untill nearest SYM_REDUCE_END
 * 
 * @param prec_stack currently used stack for terms and non-terms
 * @return number of operands 
 */
static int get_operators_num_to_handlesym(sym_stack_t * prec_stack,stack_data_t * operators)
{
    int op_num = 0;
    node * tmp = (*prec_stack);
    for(int idx = 2; tmp != NULL; op_num++) //go down until you find handle
    {
        if(op_num <= 2) operators[idx-op_num] = tmp->data;
        if(tmp->data.symbol == SYM_REDUCE_END) break;
        tmp = tmp->next;
    }
    return op_num;
}

/**
 * @brief Get the rule, support function for reduce method
 * 
 * @param prec_stack currently used stack for terms and non-terms
 * @return Current rule for reducing or NO_RULE in case of syntax error
 */
static prec_rule_t get_rule(sym_stack_t * prec_stack)
{
    //operators 
    stack_data_t operators[3];
    prec_sym_type_t sym;
    int op_num = get_operators_num_to_handlesym(prec_stack,operators);
    switch (op_num)
    {
    case 1:
        sym = peek(prec_stack).symbol;
        if(sym == SYM_ID || sym == SYM_INT || sym == SYM_FLOAT || sym == SYM_STRING || sym == SYM_NULL)
        {
            return OPERAND;
        }
        return NO_RULE;
        
    case 3:
        
        // E -> (E)
        if(operators[0].symbol == SYM_PAR_L && operators[1].symbol == NON_TERMINAL && operators[2].symbol == SYM_PAR_R)
        {
            return PARL_E_PARR;
        }
        //binary operators
        else if(operators[0].symbol == NON_TERMINAL && operators[2].symbol == NON_TERMINAL) //rules for E bin_operator E
        {
            switch (operators[1].symbol)
            {
                //arithmetic operator rules
            case SYM_PLUS: // E -> E + E
                return E_PLUS_E;
            case SYM_MINUS: // E -> E - E
                return E_MINUS_E;
            case SYM_CONCAT: // E -> E.E
                return E_CONCAT_E;
            case SYM_MUL: // E -> E * E
                return E_MUL_E;
            case SYM_DIV: // E -> E / E
                return E_DIV_E;

                //relational operator rules
            case SYM_LT: // E -> E < E
                return E_LT_E;
            case SYM_LEQ: // E -> E <= E
                return E_LEQ_E;
            case SYM_GT: // E -> E > E
                return E_GT_E;
            case SYM_GEQ: // E -> E >= E
                return E_GEQ_E;
            case SYM_EQ: // E -> E === E
                return E_EQ_E;
            case SYM_NEQ:  // E -> E !== E
                return E_NEQ_E;
            
            default:
                print_error(SYNTAX_ERR,"Missing operator.");
                return NO_RULE;
            }
        }
        else return NO_RULE;
    default:
        return NO_RULE; //for error handling
    }
}

static bool push_handle_after_last_terminal(sym_stack_t * stack)
{
    if((*stack) == NULL)
    {
        print_error(INTERNAL_ERROR, "expression.c: Stack error. \n");
        return false;
    }
    node * current_node = (*stack);
    node * prev_node = NULL;
    
    if(current_node->data.symbol != NON_TERMINAL)
    {
        stack_data_t data;
        data.symbol = SYM_REDUCE_END;
        push(stack,data);
        return true;
    }
    while (current_node != NULL)
    {
        if(current_node->data.symbol != NON_TERMINAL)
        {
            node * new_node = malloc(sizeof(struct node));
            if(new_node == NULL)
            {
                print_error(INTERNAL_ERROR,"expression.c: Fialed allocation of memmory.");
                return false;
            }
            new_node->next = current_node;
            new_node->data.symbol = SYM_REDUCE_END;
            prev_node->next = new_node;
            return true;
        }
        prev_node = current_node;
        current_node = current_node->next;
    }
    return false;
}

static int pop_stack_for_reduce(sym_stack_t * stack, stack_data_t* operators_arr, bool * stack_err)
{
    int idx = 0;
    while (*stack != NULL)
    {
        if(get_depth(stack) == 1) break;
        
        if((*stack)->data.symbol == SYM_REDUCE_END) 
        {
            pop(stack,stack_err);
            break;
        }
        operators_arr[idx] = pop(stack,stack_err);
        idx++;
    }
    return SUCCESS;
}

/**
 * @brief Get the data type of identifier by it's value
 * 
 * @return token_data_t 
 */
static type_t get_data_type_by_identifier(PData_t * pdata,error_t * err)
{
    // if(pdata->lhs == TOKEN_IDENTIFIER)
    // {   
    //     // //search for identifier in symtable and value BT_search
    //     // if(BT_Search(pdata->lhs,&pdata->token.attribute.dstr) != NULL)
    //     // {
    //     //     printf("current_data type: %d\n",pdata->token.token_type); //neprepisany token type, tak co chces overovat ty kokot
    //     //     return pdata->current->type;
    //     // }
    //     // else print_error(UNDEFINED_VARIABLE_ERR, "Line: %d: Undefined variable.\n",pdata->token.line_num);
    //     // *err = UNDEFINED_VARIABLE_ERR;
    //     // return UNDEFINED_T;

    // }
    // else
    // {
        // printf("here\n");
        switch (pdata->token.token_type)
        {
        case TOKEN_INT:
            return INT_T;
        case TOKEN_FLOAT:
            return FLOAT_T;
        case TOKEN_STRING:
            return STRING_T;
        default:
            *err = UNDEFINED_VARIABLE_ERR;
            return UNDEFINED_T;
        }
    // }
     *err = UNDEFINED_VARIABLE_ERR;
    return UNDEFINED_T;
}

/**
 * @brief 
 * 
 * @param rule 
 * @param stack 
 * @return int 
 */
static int reduce_by_rule(PData_t * pdata, prec_rule_t rule, sym_stack_t * stack)
{
    //result type in result_func
    type_t result_type = UNDEFINED_T;

    //operators
    stack_data_t operators[3]; //abort, bcs of this arr size

    //data pushed on stack
    stack_data_t data;
    bool stack_err = false;
    error_t ret_err = SUCCESS;
    if(rule == OPERAND)
    {
        pop_stack_for_reduce(stack,operators,&stack_err); 
        result_type = operators[0].data_type;
        result_type = get_data_type_by_identifier(pdata,&ret_err);
        PUSH_NON_TERM_ON_STACK(data,stack,result_type);
        return SUCCESS;
    }
    else
    {
        if(rule < NO_RULE)
        {
            pop_stack_for_reduce(stack,operators,&stack_err);
            PUSH_NON_TERM_ON_STACK(data,stack,result_type);
            return SUCCESS;
        }
        else 
        {
            pop_stack_for_reduce(stack,operators,&stack_err);
            PUSH_NON_TERM_ON_STACK(data,stack,result_type); 
            return SYNTAX_ERR;
        }
        
    }
}

static stack_data_t avoid_non_terminals(sym_stack_t * stack)
{
    node * tmp = *stack;
    while (tmp != NULL)
    {
        if(tmp->data.symbol < NON_TERMINAL)
        {
            return tmp->data;
        }
        tmp = tmp->next;
    }
    //push dollar
    stack_data_t data;
    data.symbol = SYM_DOLLAR;
    data.data_type = UNDEFINED_T;
    return data;
}

error_t precedence_analysis(PData_t * p_data)
{
    //TO DO :
    //REWRITE ALL OF THIS FOR COMPATIBILE PData_t
    error_t func_return = SUCCESS;
    bool scanned_expression = false;

    //error detection
    bool stack_err = false;
    stack_data_t operators[3];
    type_t result_type = UNDEFINED_T;
    stack_data_t non_term;
    stack_data_t debug_operator;
    debug_operator.symbol = SYM_PLUS;

    //eq and neq repair
    stack_data_t eq_or_neq;\
    eq_or_neq.symbol = SYM_EQ;\

    sym_stack_t prec_stack;
    stack_init(&prec_stack);

    //push first symbol on stack
    stack_data_t current_symbol;
    current_symbol.symbol = SYM_DOLLAR;
    current_symbol.data_type = UNDEFINED_T;
    PUSH_SYMBOL_ON_STACK(prec_stack, current_symbol);

    Token * current_token = &p_data->token; 
    current_symbol.symbol = get_symbol_from_token(current_token);

    //precednce table variables
    prec_table_idx_op_t sym_on_stack;
    prec_table_idx_op_t incoming_sym;
    prec_rule_t current_rule;

    do {
        //add function checking (expression) and null keyword
        if(current_token->token_type == TOKEN_KEYWORD && current_token->attribute.keyword != KEYWORD_NULL)
        {
            print_error(SYNTAX_ERR,"Line: %d: Unexpected keyword detected.",current_token->line_num);
            func_return = SYNTAX_ERR;
        }
        //function call
        // else if(current_token->token_type == TOKEN_IDENTIFIER)
        // {
        //     if(get_token(current_token) == TOKEN_ERR || current_token->token_type != TOKEN_PAR_L)
        //     {
        //         CLEAN_BEFORE_ERROR_EXIT(&prec_stack);
        //         return SYNTAX_ERR;
        //     }
        //     //if(defined)
        //     //recursive call of precedense analysis to function argument
        //     while (current_token->token_type != TOKEN_PAR_R)
        //     {
        //         //skip commas between expressions
        //         if(current_token->token_type != TOKEN_COMMA) precedence_analysis(p_data);
        //         GET_TOKEN_AND_REPAIR(current_token);
        //     }
        //     //else not defined to consult
            
        //     precedence_analysis(p_data);
        // }
        else if(current_token->token_type == TOKEN_DOLL) //if get dollar, expected identifier
        {
            if(get_token(current_token) == TOKEN_ERR || current_token->token_type != TOKEN_IDENTIFIER)
            {
                CLEAN_BEFORE_ERROR_EXIT(&prec_stack);
                return SYNTAX_ERR;
            }
        }

        current_symbol.symbol = get_symbol_from_token(current_token);
        current_symbol.data_type = get_data_type_by_identifier(p_data,&func_return);
        sym_on_stack = get_prec_table_operator(avoid_non_terminals(&prec_stack).symbol);
        incoming_sym = get_prec_table_operator(current_symbol.symbol);
        
        // pdata->lhs = id do ktoreho treba priradit datovy typ
        switch (prec_tab[sym_on_stack][incoming_sym])
        {
        case S:
            push_handle_after_last_terminal(&prec_stack);
            PUSH_SYMBOL_ON_STACK(prec_stack,current_symbol);
            if(get_token(current_token) == TOKEN_ERR) //EQ and NEQ fix
            {
                stack_data_t eq_or_neq;
                // printf("token type: %d\n",current_token->token_type);
                if(current_token->token_type == TOKEN_EQ) eq_or_neq.symbol = SYM_EQ;
                else if(current_token->token_type == TOKEN_NEQ) eq_or_neq.symbol = SYM_NEQ;
                func_return =  TOKEN_ERR;    
            }
            break;
        case R:
            current_rule = get_rule(&prec_stack); 
            if(current_rule == NO_RULE)
            {
                print_error(SYNTAX_ERR,"expressions.c: line %d:  No targeted rule for this expression.",current_token->line_num);
                func_return = SYNTAX_ERR;
            }
            if(reduce_by_rule(p_data,current_rule,&prec_stack) == SYNTAX_ERR)
            {
                // print_error(MISSTYPE_OPERATION_ERR,"expressions.c: line %d:  Wrong type of operands.",current_token->line_num);
                // func_return = MISSTYPE_OPERATION_ERR;
                func_return = SYNTAX_ERR;
            }
            break;
        case E:
            PUSH_SYMBOL_ON_STACK(prec_stack,current_symbol);
            GET_TOKEN_AND_REPAIR(current_token);
            break;
        case X: 
            switch (sym_on_stack)
            {
            case IDX_ID: //possible errors: |$id$id| |$id(|
                PUSH_SYMBOL_ON_STACK(prec_stack,debug_operator);
                break;
            case IDX_DOLLAR_SYMBOL: //possible errors: |$$|, |$)|
                if(incoming_sym == sym_on_stack)
                {
                    scanned_expression = true;
                    break;
                }
                print_error(SYNTAX_ERR,"Line: %d: Redundant ')'",current_token->line_num);
                GET_TOKEN_AND_REPAIR(current_token);
                break;
            case IDX_PAR_L: //possible errors: |($|
                print_error(SYNTAX_ERR,"Line: %d: Missing ')'.",current_token->line_num); //skurveny riadok - vypisany riadok dalsieho symbolu
                break;
            case IDX_PAR_R://possible errors: |)$id|, |)(|
                PUSH_SYMBOL_ON_STACK(prec_stack,debug_operator);
                break;
            default:
                scanned_expression = true;
                break;
            }
            pop_stack_for_reduce(&prec_stack,operators,&stack_err);
            PUSH_NON_TERM_ON_STACK(non_term,&prec_stack,result_type);
            func_return = SYNTAX_ERR;
            break;
            
        default:
            return SYNTAX_ERR;
        }
    }while (!scanned_expression); 
   
    CLEAN_BEFORE_ERROR_EXIT(&prec_stack);
    return func_return;
}
