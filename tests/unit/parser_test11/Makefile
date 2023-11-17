CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test parser 11"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2
run: build
	@echo "[info] performing test parser 11"
	
	./$(NAME) < ./input.txt > ./output.txt 2>./debug_output.txt || (echo -e "[info] test parser 11 - bad syntax missing cond clause [RET_CODE] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected_debug.txt ./debug_output.txt || (echo -e "[info] test parser 11 - bad syntax missing cond clause [RULE_PRINT] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected.txt ./output.txt || (echo -e "[info] test parser 11 - bad syntax missing cond clause [INSTR] \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test parser 11 - bad syntax missing cond clause \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for parser 11"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_parser11.txt		
	cp debug_output.txt ../test_artifacts/units_test_parser11_rule_print.txt