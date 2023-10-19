CC=gcc -std=c99 -g -lm
NAME=test
TEST_NAME=expression 1
TEST_OUTPUT_FILE=units_test_expression_1
PRINT=echo -e
LEAK_SEEKER=valgrind

ifeq ($(shell UNAME),Darwin)
	PRINT=printf
	LEAK_SEEKER=leaks --atExit -- 
endif 

build:
	@echo "[info] starting CC build for test $(TEST_NAME)"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test $(TEST_NAME)"
	./$(NAME) < input.txt > ./output.txt 2>&1
	# diff ./expected.txt ./output.txt || ($(PRINT) "[info] test $(TEST_NAME) \e[31mFAIL\e[0m" && exit 1)
	diff ./expected.txt ./output.txt || ($(PRINT) "[info] test $(TEST_NAME) \e[31mFAIL\e[0m")
	$(PRINT) "[info] test $(TEST_NAME) \e[32mPASS\e[0m"
	$(LEAK_SEEKER) ./$(NAME) < input.txt 

artifacts:
	@echo "[info] creating artifacts for $(TEST_NAME)"
	cp output.txt ../test_artifacts/$(TEST_OUTPUT_FILE).txt

SRC=../../../src/
local:
	$(CC) -I../../../include $(SRC)/lexical_analyzer.c $(SRC)/error.c $(SRC)/dyn_string.c $(SRC)/expression.c $(SRC)/symstack.c main.c -o $(NAME)
	./$(NAME) < input.txt

localclean:
	rm -rf *.dSYM  $(NAME)

