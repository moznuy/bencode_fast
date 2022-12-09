.PHONY: fuzzer
fuzzer:
	$(MAKE) -C fuzz all


.PHONY: clean
clean:
	$(MAKE) -C fuzz clean


.PHONY: fuzz
fuzz:
	$(MAKE) -C fuzz fuzz


.PHONY: cov
cov:
	$(MAKE) -C fuzz cov


.PHONY: format
format:
	clang-format -i bencode_fast/bencodemodule.c
	clang-format -i fuzz/fuzzer.c
	clang-format -i fuzz/standalone_fuzzer_main.c
	clang-format -i fuzz/test.c
