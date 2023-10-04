# compiler settings
CC := gcc
CFLAGS :=-Wall -Wextra -Werror -pedantic

# program settings
PROG := ifj23

#aliases and object files
SRC_FILES := $(wildcard *.c)

# rules
.PHONY: clean build submission

$(PROG): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@

submission:
	rm -rf       build
	mkdir        build
	cp include/* build
	cp src/*     build
	cp Makefile  build

build: submission
	cd build && $(MAKE)

test: submission
	rm -rf ./test_build/
	rm -rf ./test_artefacts/

	@echo "[info] creating dir for tests artefacts"
	mkdir ./test_artefacts/
	@echo "[info] starting unit tests"
	
	# for all dirs in ./tests/unit
	for f in ./tests/unit/*; do \
		if [ -d "$$f" -a $$(echo -n "$$f" | tail -c 1) != "-" ]; then \
			mkdir ./test_build/                                       && \
			cp -r ./build/* ./test_build/                             && \
			rm ./test_build/compiler.c                                && \
			cp -rf $$f/* ./test_build/                                && \
			cd ./test_build && $(MAKE) run && $(MAKE) artefacts       && \
			cd ../                                                    && \
			rm -rf ./test_build/; \
		fi \
	done

clean:
	rm -rf build
	rm -rf test_artefacts
	rm -rf test_build
