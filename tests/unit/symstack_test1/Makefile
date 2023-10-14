CC=gcc -std=c99 -g -lm
NAME=symstack

make:
	$(CC) *.c -I../../../include ../../../src/symstack.c -o $(NAME)

clean:
	rm -rf $(NAME)

run: make
	./$(NAME)
