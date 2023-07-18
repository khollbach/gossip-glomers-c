CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs --cflags json-c)

main.out: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

debug.out: main.c
	$(CC) $(CFLAGS) -g -O0 -o $@ $^ $(LDFLAGS)

.PHONY: build
build:
	docker build -t ggc .

.PHONY: run
run:
	docker run --rm -it -v $(PWD):/app ggc

.PHONY: clean
clean:
	rm -f *.out
