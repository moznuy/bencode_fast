
fuzzer:
	$(MAKE) -C fuzz all

.PHONY: fuzz

clean:
	$(MAKE) -C fuzz clean

fuzz:
	$(MAKE) -C fuzz fuzz

cov:
	$(MAKE) -C fuzz cov
