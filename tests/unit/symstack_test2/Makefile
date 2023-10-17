CC=gcc -std=c99 -g -lm
NAME=test
TEST_NAME=symstack 2
TEST_OUTPUT_FILE=units_test_symstack_2
PRINT=echo -e

ifeq ($(shell UNAME),Darwin)
	PRINT=printf
endif 

build:
	@echo "[info] starting CC build for test $(TEST_NAME)"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test $(TEST_NAME)"
	./$(NAME) < input.txt > ./output.txt 2>&1
	diff ./expected.txt ./output.txt || ($(PRINT) "[info] test $(TEST_NAME) \e[31mFAIL\e[0m" && exit 1)
	$(PRINT) "[info] test $(TEST_NAME) \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for $(TEST_NAME)"
	cp output.txt ../test_artifacts/$(TEST_OUTPUT_FILE).txt


