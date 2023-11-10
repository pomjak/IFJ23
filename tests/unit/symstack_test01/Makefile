CC=gcc -std=c99 -g -lm
NAME=test
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --error-exitcode=1 

build:
	@echo "----------------------------------------"
	@echo "[info] starting CC build for test symstack 1"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test symstack 1"
	
	if ./$(NAME) > ./output.txt 2>&1; then \
        echo -e "[info] test symstack 1 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test symstack 1 \e[31mFAIL\e[0m"; \
		exit 1;\
    fi

	@echo "[info] performing valgrind check symstack 1"

	if $(VALGRIND) ./$(NAME) > ./valgrind_output.txt 2>&1; then \
        echo -e "[info] valgrind check symstack 1 \e[32mOK\e[0m"; \
    else \
        echo -e "[info] valgrind check symstack 1 \e[33mWARN VALGRIND\e[0m"; \
    fi


artifacts:
	@echo "[info] creating artifacts for symstack 1"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_symstack1.txt	
	cp valgrind_output.txt ../test_artifacts/units_test_symstack_valgrind1.txt