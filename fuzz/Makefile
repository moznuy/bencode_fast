CC = clang
CFLAGS = -I../pythond/include/python3.11d -I../pythond/include/python3.11d -I../bencode_fast  -Wsign-compare  -g -O0 -Wall -Wextra -Wpedantic -Werror -fno-omit-frame-pointer
LDFLAGS = -L../pythond/lib -lpython3.11d -Wl,-rpath=../pythond/lib  -lpthread -ldl  -lutil -lm


.PHONY: all
all: fuzzer coverage

fuzzer: ../bencode_fast/bencodemodule.c fuzzer.c
	${CC} ${CFLAGS} ${LDFLAGS} -fsanitize=fuzzer,address fuzzer.c -o fuzzer

coverage: ../bencode_fast/bencodemodule.c fuzzer.c standalone_fuzzer_main.c
	${CC} ${CFLAGS} ${LDFLAGS} -lasan -fprofile-instr-generate -fcoverage-mapping fuzzer.c standalone_fuzzer_main.c -o coverage


.PHONY: fuzz
fuzz: fuzzer
	mkdir -p crashes
	mkdir -p CORPUS
	PYTHONHOME=$(shell pwd)/../pythond ./fuzzer -detect_leaks=1 -artifact_prefix=./crashes/ -max_len=4096 -jobs=12 -reload=1 CORPUS
# -jobs=12 -reload=1


.PHONY: cov
cov: coverage
	LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libasan.so.5 ./coverage CORPUS/*
	llvm-profdata merge -sparse *.profraw -o coverage.profdata
	llvm-cov show coverage -instr-profile=coverage.profdata -name=decode


.PHONY: test
test: test.c
	${CC} ${CFLAGS} ${LDFLAGS} -fsanitize=address test.c -o test
	./test


.PHONY: clean
clean:
	rm -r *.o crash-* leak-* fuzz-* *.prof* fuzzer test coverage __pycache__ *.so 2>/dev/null || true


.PHONY: compile
compile: ../bencode_fast/bencodemodule.c
	${CC} ${CFLAGS} -fsanitize=fuzzer,address ../bencode_fast/bencodemodule.c -c -o bencodemodule.o
