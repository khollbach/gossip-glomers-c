CFLAGS += $(shell pkg-config --cflags json-c)
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs json-c)

build/challenge-1.out: build/challenge-1.o build/util.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

build/challenge-1.o: src/challenge-1/main.c lib/util.h
	$(CC) $(CFLAGS) -c $< -o $@

build/util.o: lib/util.c lib/util.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: build
build:
	docker build -t ggc .
	mkdir -p build/

.PHONY: run
run:
	docker run --rm -it -v $(PWD):/app ggc

.PHONY: clean
clean:
	rm -rf build/*
