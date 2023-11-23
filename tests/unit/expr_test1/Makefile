CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 
export EXPR_ERROR=0

build:
	@echo "----------------------------------------"
	@echo "		expr"
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test expr 1"
	$(CC) *.c -o $(NAME) -D DEBUG -D DEBUGL=2 

run: build
	@echo "[info] performing test expr 1"
	
	if ./$(NAME) < input > ./output.txt 2>&1; then \
        echo -e "[info] test expr 1 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test expr 1 \e[31mFAIL\e[0m"; \
		export EXPR_ERROR=1;\
    fi\

	@echo "[info] performing valgrind check expr 1"

	if $(VALGRIND) ./$(NAME) <input > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check expr 1 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check expr 1 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for expr 1"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_expr1.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_expr_valgrind1.txt