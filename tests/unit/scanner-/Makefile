# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic

# program settings
PROG := scanner-test

# external libraries
UNITY := /Library/Frameworks/Unity/src
UNITY_CFILE := $(UNITY)/unity.c

# sub programs
SCANNER := ../../../src/scanner
ERROR := ../../../src/error

# rules
.PHONY: clean run

$(PROG): $(PROG).c $(ERROR).c $(SCANNER).c $(UNITY_CFILE)
	$(CC) $(CFLAGS) -I$(UNITY) $^ -o $@ 

run: $(PROG)
	./$(PROG)

clean:
	rm -rf $(PROG) *.dSYM 
