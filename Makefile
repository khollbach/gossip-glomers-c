CFLAGS += $(shell pkg-config --cflags json-c)
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs json-c)

main.out: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: build
build:
	docker build -t gcc .

.PHONY: run
run:
	docker run --rm -it -v $(PWD):/app gcc

.PHONY: clean
clean:
	rm -f *.out
