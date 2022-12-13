.PHONY: fuzzer
fuzzer:
	$(MAKE) -C fuzz all


.PHONY: clean
clean:
	$(MAKE) -C fuzz clean
	sudo rm -rf .pytest_cache *.egg-info build bencode_fast/*.so wheelhouse


.PHONY: fuzz
fuzz:
	$(MAKE) -C fuzz fuzz


.PHONY: cov
cov:
	$(MAKE) -C fuzz cov


.PHONY: format
format:
	clang-format -i --verbose bencode_fast/*.c fuzz/*.c
