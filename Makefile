CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

main.out: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
