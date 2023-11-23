CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 


build:
	@echo "[info] starting CC build for test symtable 5"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test symtable 5"
	
	if ./$(NAME) > ./output.txt 2>&1; then \
        echo -e "[info] test symtable 5 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test symtable 5 \e[31mFAIL\e[0m"; \
		exit 1;\
    fi

	@echo "[info] performing valgrind check symtable 5"

	if $(VALGRIND) ./$(NAME) > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check symtable 5 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check symtable 5 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for symtable 5"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_symtable5.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_symtable_valgrind5.txt