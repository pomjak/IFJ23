CC = gcc -std=c99 -g -lm
NAME = test
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1

build:
	@echo "----------------------------------------"
	@echo "		parser"
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test parser"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2

run: build 
	for f in ./*; do \
		if [ -d "$$f" -a "$$(echo -n "$$f" | tail -c 1)" != "-" ]; then \
			FILENAME=$$(basename $$f); \
			./$(NAME) < "$$f/$$FILENAME".swift > ./output.txt 2>debug_output.txt; \
			[[ "$$?" -eq "$$(cat "$$f/$$FILENAME".ret)" ]] || ((echo -e "[info] test parser $$FILENAME [RET_CODE] \e[31mFAIL\e[0m") && exit 1) ;\
			diff "$$f/$$FILENAME"_debug.expected ./debug_output.txt || ((echo -e "[info] test parser $$FILENAME [RULE] \e[31mFAIL\e[0m") && exit 1); \
			diff "$$f/$$FILENAME".expected ./output.txt || ((echo -e "[info] test parser $$FILENAME [INSTR] \e[31mFAIL\e[0m") && exit 1); \
			echo -e "[info] test parser $$FILENAME \e[32mOK\e[0m"; \
			$(VALGRIND) ./$(NAME) < "$$f/$$FILENAME".swift > ./valgrind_output.txt 2>&1 || \
			echo -e "[info] valgrind check $$FILENAME \e[32mOK\e[0m" || \
			echo -e "[info] valgrind check $$FILENAME \e[33mWARN VALGRIND\e[0m"; \
			echo -e "----------------------------------------";\
			cp output.txt ../test_artifacts/units_test_parser_"$$FILENAME".txt;\
			cp debug_output.txt ../test_artifacts/units_test_parser_"$$FILENAME"_debug.txt;\
			cp valgrind_output.txt ../test_artifacts/units_test_parser_"$$FILENAME"_valgrind.txt;\
		fi \
	done

artifacts:
