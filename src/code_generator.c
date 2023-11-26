/**
 * @name IFJ23
 * @file code_generator.c
 * @brief Code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 26.11.2023
 **/

#include "code_generator.h"
#include <stdio.h>

#define BUFFER_PRINT(fmt, ...) {                               \
    int   sprintf_size = snprintf(NULL, 0, fmt, ##__VA_ARGS__);\
    char* sprintf_data = malloc(sprintf_size + 1);             \
    sprintf(sprintf_data, fmt, ##__VA_ARGS__);                 \
    code_generator_buffer_print(sprintf_data);                  \
}

unsigned tmp_var_id = 0;
unsigned func_param_id = 0;
unsigned for_open = 0;
dstring_t print_buffer;

void code_generator_defvar(char *frame, char *varname, unsigned id){
    printf("\nDEFVAR %s@%s_%d\n",frame,varname, id);
}

bool code_generator_need_function_frame(char* name) {
    const char* no_frame_funcions[] = {"readString", "readInt", "readDouble", "write", "Int2Double", "Double2Int", "length", "chr"};
    const unsigned no_frame_funcions_count = 8;

    for(unsigned i = 0; i < no_frame_funcions_count; i++){
        if(strcmp(name, no_frame_funcions[i]) == 0) {
            return false;  
        }
    }

    return true;
}

void code_generator_buffer_print(char *text){
    if(for_open > 0){
        dstring_add_const_str(&print_buffer, text);
    } else {
        printf(text);
    }
}

void code_generator_buffer_transmit(){
    printf(print_buffer.str);
    dstring_clear(&print_buffer);
}

void code_generator_prolog(){
    dstring_init(&print_buffer);
	printf(".IFJcode23\n");
	code_generator_defvar("GF", "PARAM", 1);
	code_generator_defvar("GF", "PARAM", 2);
	code_generator_defvar("GF", "RESULT", 1);
    code_generator_defvar("GF", "LENGTH", 1);
    code_generator_defvar("GF", "READED", 1);
    code_generator_defvar("GF", "READED", 2);
    code_generator_defvar("GF", "READED", 3);
    code_generator_defvar("GF", "INT2CHAR", 1);
    code_generator_createframe();
    code_generator_pushframe();
    code_generator_function_ord();
    code_generator_substring();
}

/**
 * If
 */

void code_generator_if_header(unsigned id){
    BUFFER_PRINT("\nPUSHS bool@true\n");
    BUFFER_PRINT("JUMPIFNEQS $$ELSE_%u\n", id);
}

void code_generator_if_else(unsigned id){
	BUFFER_PRINT("\nJUMP $$IF_END_%u\n", id);
	BUFFER_PRINT("LABEL $$ELSE_%u\n", id);
}

void code_generator_if_end(unsigned id){
	BUFFER_PRINT("\nLABEL $$IF_END_%u\n", id);
}

/**
 * Variable
 */

void code_generator_var_assign_token(token_T token){
    code_generator_var_assign(token.value.string_val.str);
}

void code_generator_var_assign(char* var){
    
	if(strcmp(var, "_") != 0){
		BUFFER_PRINT("\nPOPS LF@%s_%d\n", var, 0);
	} else{
        code_generator_defvar("LF","TMP", tmp_var_id);
		BUFFER_PRINT("POPS LF@TMP_%u\n", tmp_var_id);
		tmp_var_id++;
	}
}

void code_generator_var_declare_token(token_T token){
    code_generator_var_declare(token.value.string_val.str);
}

void code_generator_var_declare(char* variable){
	code_generator_defvar("LF", variable, 0);
    BUFFER_PRINT("POPS LF@%s_%d\n", variable, 0);
}

void code_generator_copy_var_to_new_frame(char* variable){
    code_generator_defvar("TF", variable, 0);
    BUFFER_PRINT("MOVE TF@%s_%d LF@%s_%d\n", variable, 0, variable, 0);
}

void code_generator_eof(){
	BUFFER_PRINT("\nLABEL $$EOF\n");
    dstring_free(&print_buffer);
}

void code_generator_push(token_T token){
    if(token.type != TOKEN_IDENTIFIER &&
       token.type != TOKEN_NIL &&
       token.type != TOKEN_INT &&
       token.type != TOKEN_DBL &&
       token.type != TOKEN_STRING) { 
		return; 
	}
       
    BUFFER_PRINT("\nPUSHS ");
    code_generator_print_value(token);
    BUFFER_PRINT("\n");
}

/**
 * For loop
 */

void code_generator_for_loop_end(unsigned id){
    BUFFER_PRINT("\nJUMP $$FOR_%u\n",id);
    BUFFER_PRINT("LABEL $$FOR_END_%u\n",id);

    for_open--;
    if(for_open <= 0){
        code_generator_buffer_transmit();
    }
}

void code_generator_for_loop_if(unsigned id){
	BUFFER_PRINT("\nPUSHS bool@true\n");
    BUFFER_PRINT("JUMPIFNEQS $$FOR_END_%u\n", id);
	BUFFER_PRINT("JUMP $$FOR_BODY_%u\n", id);
}

void code_generator_for_label(unsigned id){
    for_open++;
	BUFFER_PRINT("\nLABEL $$FOR_%u\n", id);
}

void code_generator_for_body(unsigned id){
	BUFFER_PRINT("\nLABEL $$FOR_BODY_%u\n", id);
}

/**
 * Value
 */

void code_generator_print_value(token_T token){

    if(token.type == TOKEN_IDENTIFIER){
        BUFFER_PRINT("LF@%s_%d",token.value.string_val.str, 0);
    } else if (token.type == TOKEN_NIL) {
	    BUFFER_PRINT("nil@nil");
    } else if (token.type == TOKEN_INT) {
	    BUFFER_PRINT("int@%d",token.value.int_val);
	} else if (token.type == TOKEN_DBL) {
		BUFFER_PRINT("float@%a",token.value.double_val);
	} else if (token.type == TOKEN_STRING) {
		BUFFER_PRINT("string@");
        int token_length = strlen(token.value.string_val.str);

        for(int i = 0; i < token_length; i++){
            char c = token.value.string_val.str[i];
            if((c >= 0 && c <= 32) || c == '#' || c == '\\'){
                BUFFER_PRINT("\\%03d",c);
            } else{
                BUFFER_PRINT("%c", c);
            }
        }
	}
}

/**
 * Frame
 */

void code_generator_pushframe(){
    BUFFER_PRINT("\nPUSHFRAME\n");
}

void code_generator_popframe(){
    BUFFER_PRINT("\nPOPFRAME\n");
}

void code_generator_createframe(){
    BUFFER_PRINT("\nCREATEFRAME\n");
}

/**
 * Operations
 */

void code_generator_operations(token_type_T operator, bool is_int){

    if(operator == TOKEN_ADD) {
        BUFFER_PRINT("\nADDS\n");
    } else if (operator == TOKEN_SUB) {
        BUFFER_PRINT("\nSUBS\n");
    } else if (operator == TOKEN_MUL) {
        BUFFER_PRINT("\nMULS\n");
    } else if (operator == TOKEN_DIV && !is_int) {
        BUFFER_PRINT("\nDIVS\n");
    } else if (operator == TOKEN_DIV && is_int) {
        BUFFER_PRINT("\nIDIVS\n");
    } else if (operator == TOKEN_LT) {
        BUFFER_PRINT("\nLTS\n");
    } else if (operator == TOKEN_LEQ) {
        BUFFER_PRINT("\nGTS\n");
        BUFFER_PRINT("\nNOTS\n");
    } else if (operator == TOKEN_GT) {
        BUFFER_PRINT("\nGTS\n");
    } else if (operator == TOKEN_GEQ) {
        BUFFER_PRINT("\nLTS\n");
        BUFFER_PRINT("\nNOTS\n");
    } else if (operator == TOKEN_EQ) {
        BUFFER_PRINT("\nEQS\n");
    } else if (operator == TOKEN_NEQ) {
        BUFFER_PRINT("\nEQS\n");
        BUFFER_PRINT("\nNOTS\n");
    }
}

void code_generator_concats(){
    //POPS PARAM_2
    BUFFER_PRINT("\nPOPS GF@PARAM_2\n");

    //POPS PARAM_1
    BUFFER_PRINT("POPS GF@PARAM_1\n");

    //CONCAT: RESULT = PARAM_1 + PARAM_2
    BUFFER_PRINT("CONCAT GF@RESULT_1 GF@PARAM_1 GF@PARAM_2\n");

    //PUSHS RESULT
    BUFFER_PRINT("PUSHS GF@RESULT_1\n");
}

void code_generator_clears(){
    BUFFER_PRINT("\nCLEARS\n");
}

/**
 * Function
 */

void code_generator_function_call_token(token_T token){
    code_generator_function_call(token.value.string_val.str);
}

void code_generator_function_call(char* name){

    func_param_id = 0;

    if(strcmp(name,"readString") == 0){
        BUFFER_PRINT("READ GF@READED_1 string\n");
        BUFFER_PRINT("PUSHS GF@READED_1\n");
    } else if(strcmp(name,"readInt") == 0){
        BUFFER_PRINT("READ GF@READED_2 int\n");
        BUFFER_PRINT("PUSHS GF@READED_2\n");
    } else if((strcmp(name,"readDouble") == 0)){
        BUFFER_PRINT("READ GF@READED_3 float\n");
        BUFFER_PRINT("PUSHS GF@READED_3\n");
    } else if (code_generator_need_function_frame(name)) {
        BUFFER_PRINT("CALL $$FUNCTION_%s\n", name);
    }
}

void code_generator_function_call_param_add_token(token_T token_name, token_T token_value){
    code_generator_function_call_param_add(token_name.value.string_val.str, token_value);
}

void code_generator_function_call_param_add(char* name, token_T token){

    if(code_generator_need_function_frame(name)) {
        if (func_param_id == 0) {
            code_generator_createframe();
        }

        code_generator_defvar("TF", "??", func_param_id);
        BUFFER_PRINT("MOVE TF@??_%d ", func_param_id);
        code_generator_print_value(token);
        BUFFER_PRINT("\n");

        func_param_id++;
    }

    if(strcmp(name,"write") == 0){
        BUFFER_PRINT("WRITE ");
        code_generator_print_value(token);
        BUFFER_PRINT("\n");
    } else if(strcmp(name,"Int2Double") == 0){
        code_generator_push(token);
        BUFFER_PRINT("INT2FLOATS\n");  
    } else if((strcmp(name,"Double2Int") == 0)){
        code_generator_push(token);
        BUFFER_PRINT("FLOAT2INTS\n");
    } else if((strcmp(name,"length") == 0)){
        BUFFER_PRINT("STRLEN GF@LENGTH_1 ");
        code_generator_print_value(token);
        BUFFER_PRINT("\n");
        BUFFER_PRINT("PUSHS GF@LENGTH_1\n");
    } else if(strcmp(name, "chr") == 0){
        BUFFER_PRINT("INT2CHAR GF@INT2CHAR_1 ");
        code_generator_print_value(token);
        BUFFER_PRINT("\n");
        BUFFER_PRINT("PUSHS GF@INT2CHAR_1\n");
    }
}

void code_generator_function_ord(){
    code_generator_function_label("ord");

    code_generator_defvar("LF", "length", 0);
    printf("STRLEN LF@length_0 LF@??_0\n");

    printf("JUMPIFNEQ ORD_NOT0 int@0 LF@length_0\n");
    printf("PUSHS int@0\n");
    code_generator_return();

    printf("LABEL ORD_NOT0\n");
    code_generator_defvar("LF", "ord_value", 0);
    printf("STRI2INT LF@ord_value_0 LF@??_0 int@0\n");
    printf("PUSHS LF@ord_value_0\n");

    code_generator_function_end("ord");
}

void code_generator_substring(){
    code_generator_function_label("substring");

    code_generator_defvar("LF", "length", 0);
    printf("STRLEN LF@length_0 LF@??_0\n");

    code_generator_defvar("LF", "condition", 0);

    printf("LT LF@condition_0 LF@??_1 int@0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("LT LF@condition_0 LF@??_2 int@0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("GT LF@condition_0 LF@??_1 LF@??_2\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("LT LF@condition_0 LF@??_1 LF@length_0\n");
    printf("JUMPIFNEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("GT LF@condition_0 LF@??_2 LF@length_0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    code_generator_defvar("LF", "result", 0);
    code_generator_defvar("LF", "char", 0);

    printf("MOVE LF@result_0 string@\n");

    printf("LABEL SUSTRING_loop\n");

    printf("LT LF@condition_0 LF@??_1 LF@??_2\n");
    printf("JUMPIFNEQ SUSTRING_loop_end LF@condition_0 bool@true\n");

    printf("GETCHAR LF@char_0 LF@??_0 LF@??_1\n");
    printf("CONCAT LF@result_0 LF@result_0 LF@char_0\n");
    printf("ADD LF@??_1 int@1 LF@??_1\n");

    printf("JUMP SUSTRING_loop\n");
    printf("LABEL SUSTRING_loop_end\n");

    printf("PUSHS LF@result_0\n");
    code_generator_return();

    printf("LABEL SUSTRING_nil\n");
    printf("PUSHS nil@nil\n");

    code_generator_function_end("substring");
}

void code_generator_function_label_token(token_T token){
    code_generator_function_label(token.value.string_val.str);
}

void code_generator_function_label(char* name){
    BUFFER_PRINT("\nJUMP $$FUNCTION_END_%s\n", name);
    BUFFER_PRINT("\nLABEL $$FUNCTION_%s\n", name);
    code_generator_pushframe();
}

void code_generator_param_map(char *param_name, unsigned param_id){
    code_generator_defvar("LF", param_name, 0);
    BUFFER_PRINT("MOVE LF@%s_%d LF@??_%d\n", param_name, 0, param_id);
}

void code_generator_function_end(char* name){
    code_generator_popframe();
    BUFFER_PRINT("RETURN\n");
    BUFFER_PRINT("\nLABEL $$FUNCTION_END_%s\n", name);
}

void code_generator_return(){
    code_generator_popframe();
    BUFFER_PRINT("RETURN\n");
}