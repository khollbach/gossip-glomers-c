CFLAGS += $(shell pkg-config --cflags json-c)
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs json-c)

main.out: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
