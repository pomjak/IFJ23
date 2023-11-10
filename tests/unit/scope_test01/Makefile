CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 

build:
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test scope 1"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test scope 1"
	
	if ./$(NAME) > ./output.txt 2>&1; then \
        echo -e "[info] test scope 1 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test scope 1 \e[31mFAIL\e[0m"; \
		exit 1;\
    fi

	@echo "[info] performing valgrind check scope 1"

	if $(VALGRIND) ./$(NAME) > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check scope 1 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check scope 1 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for scope 1"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_scope1.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_scope_valgrind1.txt