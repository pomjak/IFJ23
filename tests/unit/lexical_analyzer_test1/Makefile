CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test lexical analyzer 1"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test lexical analyser 1"
	./$(NAME) < ./input.txt > ./output.txt 2>&1
	diff ./expected.txt ./output.txt || (echo -e "[info] test lexical analyzer 1 FAIL" && exit 1)
	@echo "[info] test lexical analyzer 1 PASS"

artifacts:
	@echo "[info] creating artifacts for lexical analyzer 1"
	cp output.txt ../test_artifacts/units_test_lexical_analyzer1.txt


