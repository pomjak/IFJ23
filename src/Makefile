# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic

# program settings
PROG := compiler

# sub programs
SCANNER := scanner
PARSER := parser
SYMSTACK := symstack
ERROR := error

#aliases and object files
SRC_FILES := $(wildcard *.c)

# rules
.PHONY: clean scanner parser symstack

$(PROG): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@

scanner: $(ERROR).c $(SCANNER).c
	$(CC) $(CFLAGS) -DSCANNER_TEST $^ -o $@

parser: $(ERROR).c $(PARSER).c
	$(CC) $(CFLAGS) -DPARSER_TEST $^ -o $@

symstack: $(ERROR).c $(SYMSTACK).c
	$(CC) $(CFLAGS) -DSYMSTACK_TEST $^ -o $@

clean:
	rm -f $(PROG) *.dSYM $(SCANNER) $(PARSER) $(SYMSTACK) 
