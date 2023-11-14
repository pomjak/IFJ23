CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test parser 4"
	$(CC) *.c -o $(NAME) -D RULEDEBUG -D DEBUG -D DEBUGL=2
run: build
	@echo "[info] performing test parser 5"
	
	./$(NAME) < ./input.txt > ./output.txt 2>&1

artifacts:
	@echo "[info] creating artifacts for parser 5"
	@echo "----------------------------------------"
	cp output.txt ../test_artifacts/units_test_parser5.txt	