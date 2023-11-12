CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test parser 1"
	$(CC) $(CFLAGS) -g -D DEBUG -D DEBUGL=$(level) $^ -o $(NAME)

run: build
	@echo "[info] performing test parser 1"
	
	if ./$(NAME) < input.txt > ./output.txt 2>&1; then \
		diff ./expected.txt ./output.txt || (echo -e "[info] test parser 1 \e[31mFAIL\e[0m" && exit 1)\
        echo -e "[info] test parser 1 \e[32mPASS\e[0m"; \
    else \
        echo -e "[info] test parser 1 \e[31mFAIL\e[0m"; \
		exit 1;\
    fi


artifacts:
	@echo "[info] creating artifacts for parser 1"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_parser1.txt	