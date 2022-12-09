CFLAGS = -I../pythond/include/python3.11d -I../pythond/include/python3.11d -I../bencode_fast  -Wsign-compare  -g -O0 -Wall -Wextra -Wpedantic
LDFLAGS = -L../pythond/lib -lpython3.11d -Wl,-rpath=../pythond/lib  -lpthread -ldl  -lutil -lm  


.PHONY: all
all: fuzzer coverage

fuzzer: ../bencode_fast/bencodemodule.c fuzzer.c
	clang ${CFLAGS} ${LDFLAGS} -fsanitize=fuzzer,address fuzzer.c -o fuzzer

coverage: ../bencode_fast/bencodemodule.c fuzzer.c standalone_fuzzer_main.c
	clang ${CFLAGS} ${LDFLAGS} -fprofile-instr-generate -fcoverage-mapping fuzzer.c standalone_fuzzer_main.c -o coverage


.PHONY: fuzz
fuzz: fuzzer
	mkdir -p crashes
	mkdir -p CORPUS
	PYTHONHOME=$(shell pwd)/../pythond ./fuzzer -detect_leaks=0 -artifact_prefix=./crashes/ CORPUS


.PHONY: cov
cov: coverage
	./coverage CORPUS/*
	llvm-profdata merge -sparse *.profraw -o coverage.profdata
	llvm-cov show coverage -instr-profile=coverage.profdata -name=decode


.PHONY: test
test: test.c
	clang ${CFLAGS} ${LDFLAGS} test.c -o test
	./test


.PHONY: clean
clean:
	rm -r *.o crash-* leak-* fuzz-* *.prof* fuzzer test coverage __pycache__ *.so 2>/dev/null || true