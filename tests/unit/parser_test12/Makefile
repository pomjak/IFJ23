CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 

build:
	@echo "[info] starting CC build for test parser 12"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2
run: build
	@echo "[info] performing test parser 12"
	
	./$(NAME) < ./input.txt > ./output.txt 2>./debug_output.txt || (echo -e "[info] test parser 12 - func_decl with body, func calls and eols [RET_CODE] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected_debug.txt ./debug_output.txt || (echo -e "[info] test parser 12 - func_decl with body, func calls and eols [RULE_PRINT] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected.txt ./output.txt || (echo -e "[info] test parser 12 - func_decl with body, func calls and eols [INSTR]\e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test parser 12 - func_decl with body, func calls and eols \e[32mPASS\e[0m"
	
	@echo "[info] performing valgrind check test parser 12"

	if $(VALGRIND) ./$(NAME)< ./input.txt > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind test parser 12 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind test parser 12 \e[33mWARN VALGRIND\e[0m"; \
    fi

artifacts:
	@echo "[info] creating artifacts for parser 12"
	@echo "----------------------------------------"
	cp output.txt 		../test_artifacts/units_test_parser12.txt
	cp debug_output.txt ../test_artifacts/units_test_parser12_rule_print.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_parser12_valgrind.txt