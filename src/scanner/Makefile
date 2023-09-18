# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic
PROG := scanner

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) -DSCANNER_TEST $^ -o $@

run: $(PROG)
	./$(PROG) $(ARGS)

clean:
	rm $(PROG)
