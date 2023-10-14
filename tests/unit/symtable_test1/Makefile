CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test symtable 1"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test symtable 1"
	./$(NAME) > ./output.txt 2>&1
	diff ./expected.txt ./output.txt || (echo -e "[info] test symtable 1 \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test symtable 1 \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for symtable 1"
	cp output.txt ../test_artifacts/units_test_symtable1.txt