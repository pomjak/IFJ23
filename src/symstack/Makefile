# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic
PROG := symstack

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) -DSYMSTACK_TEST $^ -o $@

run: $(PROG)
	./$(PROG) $(ARGS)

clean:
	rm $(PROG)
