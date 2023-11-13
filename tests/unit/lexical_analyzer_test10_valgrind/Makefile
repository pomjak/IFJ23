CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test lexical analyzer 10 valgrind"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test lexical analyser 10 valgrind"
	valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./$(NAME) < ./input.txt > ./output.txt 2>&1 || (echo -e "[info] test lexical analyzer 10 valgrind \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test lexical analyzer 10 valgrind \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for lexical analyzer 10 valgrind"
	cp output.txt ../test_artifacts/units_test_lexical_analyzer10_valgrind.txt