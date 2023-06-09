# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic

# program settings
PROG := compiler
OBJ := obj

#aliases and object files
SRC := src
SRC_FILES := $(wildcard $(SRC)/*.c)
SRC_OBJ := $(patsubst $(SRC)/%.c, $(OBJ)/%.o ,$(SRC_FILES))

# components settings
PARSER := $(SRC)/parser
PARSER_FILES := $(wildcard $(PARSER)/*.c)
PARSER_OBJ := $(patsubst $(PARSER)/%.c, $(OBJ)/%.o, $(PARSER_FILES))

SCANNER := $(SRC)/scanner
SCANNER_FILES := $(wildcard $(SCANNER)/*.c)
SCANNER_OBJ := $(patsubst $(SCANNER)/%.c, $(OBJ)/%.o, $(SCANNER_FILES))

STACK := $(SRC)/symstack
STACK_FILES := $(wildcard $(STACK)/*.c)
STACK_OBJ := $(patsubst $(STACK)/%.c, $(OBJ)/%.o, $(STACK_FILES))

# includes
I_STACK := -Isrc/symstack
I_SCANNER := -Isrc/scanner
I_PARSER := _Isrc/parser

# rules
.PHONY: all clean

all: $(PROG)
	@echo Compilation succesful!

$(PROG): $(SRC_OBJ)  $(SCANNER_OBJ) $(PARSER_OBJ) $(STACK_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# linker rules
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# components compilation
$(OBJ)/%.o: $(PARSER)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SCANNER)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(STACK)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -r $(PROG) $(OBJ)
