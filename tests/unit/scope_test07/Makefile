CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 

build:
	@echo "[info] starting CC build for test scope 7"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test scope 7"
	
	if ./$(NAME) > ./output.txt 2>&1; then \
        echo -e "[info] test scope 7 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test scope 7 \e[31mFAIL\e[0m"; \
		exit 1;\
    fi

	@echo "[info] performing valgrind check scope 7"

	if $(VALGRIND) ./$(NAME) > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check scope 7 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check scope 7 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for scope 7"
	cp output.txt ../test_artifacts/units_test_scope7.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_scope_valgrind7.txt