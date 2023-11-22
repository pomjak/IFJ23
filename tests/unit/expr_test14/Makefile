CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 

build:
	@echo "[info] starting CC build for test expr 14"
	$(CC) *.c -o $(NAME) -D DEBUG -D DEBUGL=2 

run: build
	@echo "[info] performing test expr 14"
	
	if ./$(NAME) < input > ./output.txt 2>&1; then \
        echo -e "[info] test expr 14 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test expr 14 \e[31mFAIL\e[0m"; \
		export EXPR_ERROR=1; \
    fi

	@echo "[info] performing valgrind check expr 14"

	if $(VALGRIND) ./$(NAME) <input > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check expr 14 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check expr 14 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for expr 14"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_expr14.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_expr_valgrind14.txt