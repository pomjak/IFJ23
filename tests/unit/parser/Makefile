CC := gcc
CFLAGS := -std=c99 -g -lm
NAME := test
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1
ERROR=0
DIRECTORIES := $(filter-out %/., $(wildcard */))

.PHONY: run 

build: $(OBJ)
	@echo "----------------------------------------"
	@echo "		parser"
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test parser"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUGL=2 $(CFLAGS)


run: build
	$(foreach dir,$(DIRECTORIES), \
		if [ -d "$(dir)" -a "$$(echo -n "$(dir)" | tail -c 1)" != "-" ]; then \
			FILENAME=$$(basename $(dir)); \
			./$(NAME) < "$(dir)/$${FILENAME}".swift > ./output.txt 2>debug_output.txt; \
			RET_CODE=$$?; \
			echo -e "ret:"$${RET_CODE}" exp:"$$(cat "$(dir)/$${FILENAME}".ret)""; \
			{ [ "$${RET_CODE}" -eq "$$(cat "$(dir)/$${FILENAME}".ret)" ]; } || \
				{ echo -e "[info] test parser $${FILENAME} [RET_CODE] \e[31mFAIL\e[0m" && exit 1;}; \
			mkdir -p ../test_artifacts/"parser_$${FILENAME}"/ ; \
			cp output.txt ../test_artifacts/"parser_$${FILENAME}"/"$${FILENAME}".txt; \
			cp debug_output.txt ../test_artifacts/"parser_$${FILENAME}"/"$${FILENAME}"_debug.txt; \
			{ diff "./$(dir)$${FILENAME}_debug.expected" ./debug_output.txt; } || \
				{ echo -e "[info] test parser $${FILENAME} [RULE] \e[31mFAIL\e[0m" && exit 1; }; \
			echo -e "[info] test parser $${FILENAME} \e[32mOK\e[0m"; \
			$(VALGRIND) ./$(NAME) < "$(dir)/$${FILENAME}.swift" > ./valgrind_output.txt 2>&1 || \
				{ echo -e "[info] valgrind check $${FILENAME} \e[32mOK\e[0m" || \
			echo -e "[info] valgrind check $${FILENAME} \e[33mWARN VALGRIND\e[0m"; }; \
			cp valgrind_output.txt ../test_artifacts/"parser_$${FILENAME}"/"$${FILENAME}"_valgrind.txt; \
			echo -e "----------------------------------------"; \
		fi; \
	)
	

artifacts:

