# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic
PROG := parser

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) -DPARSER_TEST $^ -o $@

run: $(PROG)
	./$(PROG) $(ARGS)

clean:
	rm $(PROG)
