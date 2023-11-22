CC=gcc -std=c99 -g -lm
NAME=test

build:
	@echo "[info] starting CC build for test code generator 3"
	$(CC) *.c -o $(NAME)

run: build
	@echo "[info] performing test code generator 3"
	./$(NAME) > ./output.txt 2>&1
	./ic23int ./output.txt < input.txt > ./runOutput.txt 2>&1
	diff ./expected.txt ./runOutput.txt || (echo -e "[info] test code generator 3 \e[31mFAIL\e[0m" && exit 1)
	echo -e "[info] test code generator 3 \e[32mPASS\e[0m"

artifacts:
	@echo "[info] creating artifacts for code generator 3"
	cp output.txt ../test_artifacts/units_test_code_generator3_asm.txt
	cp runOutput.txt ../test_artifacts/units_test_code_generator3_out.txt