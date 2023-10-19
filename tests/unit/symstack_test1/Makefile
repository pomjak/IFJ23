CC=gcc -std=c99 -g -lm
NAME=test
TEST_NAME=symstack 1
TEST_OUTPUT_FILE=units_test_symstack_1

# DIFFERENT OS SETTINGS
PRINT=echo -e
LEAK_SEEKER=valgrind
NEW_LINE=


ifeq ($(shell UNAME),Darwin)
	PRINT=printf 
	LEAK_SEEKER=leaks -quiet --atExit --  
	NEW_LINE=\n
endif 

build:
	@echo "[info] starting CC build for test $(TEST_NAME)"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test $(TEST_NAME)"
	

	if $(LEAK_SEEKER) ./$(NAME) <input.txt > /dev/null 2>&1; then \
        $(PRINT) "[info] leak test $(TEST_NAME) \033[32mPASS\033[0m $(NEW_LINE)"; \
    else \
        $(PRINT) "[info] test $(TEST_NAME) \e[31mFAIL\e[0m $(NEW_LINE)"; \
		exit 1;\
    fi

	./$(NAME) < input.txt > ./output.txt 2>&1
	diff ./expected.txt ./output.txt || ($(PRINT) "[info] test $(TEST_NAME) \e[31mFAIL\e[0m $(NEW_LINE)" && exit 1)
	$(PRINT) "[info] test $(TEST_NAME) \e[32mPASS\e[0m $(NEW_LINE)"
	
	

artifacts:
	@echo "[info] creating artifacts for $(TEST_NAME)"
	cp output.txt ../test_artifacts/$(TEST_OUTPUT_FILE).txt
