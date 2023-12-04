CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test code generator 10"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test code generator 10"
	./$(NAME) > ./output.txt 2>&1
	./ic23int ./output.txt < input.txt > ./runOutput.txt 2>&1
	diff ./expected.txt ./runOutput.txt || (echo -e "[info] test code generator 10 \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test code generator 10 \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for code generator 10"
	cp output.txt ../test_artifacts/units_test_code_generator10_asm.txt
	cp runOutput.txt ../test_artifacts/units_test_code_generator10_out.txt