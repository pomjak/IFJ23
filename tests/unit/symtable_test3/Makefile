CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test symtable 3"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test symtable 3"
	./$(NAME) > ./output.txt 2>&1 || (echo -e "[info] test symtable 3 \e[31mFAIL\e[0m" ; cat ./output.txt && exit 1)
	valgrind --leak-check=full --verbose -s ./$(NAME) > ./valgrind_output.txt 2>&1
	echo -e "[info] test symtable 3 \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for symtable 3"
	cp output.txt ../test_artifacts/units_test_symtable3.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_symtable_valgrind3.txt