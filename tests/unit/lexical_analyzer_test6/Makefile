CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test lexical analyzer 6"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test lexical analyser 6"
	./$(NAME) < ./input.txt > ./output.txt 2>&1
	diff ./expected.txt ./output.txt || (echo -e "[info] test lexical analyzer 6 \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test lexical analyzer 6 \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for lexical analyzer 6"
	cp output.txt ../test_artifacts/units_test_lexical_analyzer6.txt