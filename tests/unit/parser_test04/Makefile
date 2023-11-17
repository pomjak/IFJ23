CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test parser 4"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2
run: build
	@echo "[info] performing test parser 4"
	
	./$(NAME) < ./input.txt > ./output.txt 2>./debug_output.txt
	diff ./expected_debug.txt ./debug_output.txt || (echo -e "[info] test parser 04 - function declaration param processing [RULE_PRINT] \e[31mFAIL\e[0m" && exit 1)
	diff ./expected.txt ./output.txt || (echo -e "[info] test parser 04 - function declaration param processing [INSTR] \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test parser 04 - function declaration param processing \e[32mPASS\e[0m"


artifacts:
	@echo "[info] creating artifacts for parser 4"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_parser4.txt		
	cp output.txt ../test_artifacts/units_test_parser4_rule_print.txt	