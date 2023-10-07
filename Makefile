# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic -lm

# program settings
PROG := ifj23

#aliases and object files
SRC_FILES := $(wildcard *.c)

# rules
.PHONY: clean build submission

$(PROG): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(PROG)-debug: $(SRC_FILES)
	$(CC) $(CFLAGS) -g -D DEBUG $^ -o $(PROG)-debug

submission:
	rm -rf       build
	mkdir        build
	cp include/* build
	cp src/*     build
	cp Makefile  build

build: submission
	cd build && $(MAKE)

debug: submission
	cd build && $(MAKE) $(PROG)-debug

test: submission
	rm -rf ./test_build/
	rm -rf ./test_artifacts/

	@echo "[info] creating dir for tests artifacts"
	mkdir ./test_artifacts/
	@echo "[info] starting unit tests"
	
	# for all dirs in ./tests/unit
	for f in ./tests/unit/*; do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" ]; then \
			mkdir ./test_build/                                       && \
			cp -r ./build/* ./test_build/                             && \
			rm ./test_build/compiler.c                                && \
			cp -rf $$f/* ./test_build/                                && \
			cd ./test_build && $(MAKE) run && $(MAKE) artifacts       && \
			cd ../                                                    && \
			rm -rf ./test_build/ || exit 1;\
		fi \
	done

clean:
	rm -rf build
	rm -rf test_artifacts
	rm -rf test_build
