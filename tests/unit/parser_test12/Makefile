CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test parser 12"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2
run: build
	@echo "[info] performing test parser 12"
	
	./$(NAME) < ./input.txt > ./output.txt 2>./debug_output.txt || (echo -e "[info] test parser 012 - func_decl with body ,func calls and eols [RET_CODE] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected_debug.txt ./debug_output.txt || (echo -e "[info] test parser 012 - func_decl with body ,func calls and eols [RULE_PRINT] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected.txt ./output.txt || (echo -e "[info] test parser 012 - func_decl with body ,func calls and eols [INSTR]\e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test parser 01 - empty input file \e[32mPASS\e[0m"


artifacts:
	@echo "[info] creating artifacts for parser 12"
	@echo "----------------------------------------"
	cp output.txt 		../test_artifacts/units_test_parser12.txt
	cp debug_output.txt ../test_artifacts/units_test_parser12_rule_print.txt	