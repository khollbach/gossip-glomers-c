CFLAGS += $(shell pkg-config --cflags json-c)
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs json-c)
SRC_DIR := src
BUILD_DIR:= build
LIB_DIR := lib

# Generate challenge executables
CHALLENGES := challenge-1 challenge-2
CHALLENGE_EXECS := $(patsubst %, $(BUILD_DIR)/%.out, $(CHALLENGES))

# Generate source files for each challenge
CHALLENGE_1_SRC := $(SRC_DIR)/challenge-1/challenge-1.c
CHALLENGE_2_SRC := $(SRC_DIR)/challenge-2/challenge-2.c

# Generate object files for each challenge
CHALLENGE_1_OBJS := $(patsubst $(SRC_DIR)/challenge-1/%.c, $(BUILD_DIR)/%.o, $(CHALLENGE_1_SRC))
CHALLENGE_2_OBJS := $(patsubst $(SRC_DIR)/challenge-2/%.c, $(BUILD_DIR)/%.o, $(CHALLENGE_2_SRC))


all: $(CHALLENGE_EXECS)

$(BUILD_DIR)/challenge-1.out: $(CHALLENGE_1_OBJS) $(BUILD_DIR)/util.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/challenge-2.out: $(CHALLENGE_2_OBJS) $(BUILD_DIR)/util.o $(BUILD_DIR)/collections.o $(BUILD_DIR)/tcp.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/challenge-1.o: $(CHALLENGE_1_SRC) $(LIB_DIR)/util.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/challenge-2.o: $(CHALLENGE_2_SRC) $(LIB_DIR)/util.h $(LIB_DIR)/collections.h $(LIB_DIR)/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

# Generate object files for library modules
$(BUILD_DIR)/util.o: $(LIB_DIR)/util.c $(LIB_DIR)/util.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/collections.o: $(LIB_DIR)/collections.c $(LIB_DIR)/collections.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tcp.o: $(LIB_DIR)/tcp.c $(LIB_DIR)/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: build
build:
	docker build -t ggc .
	mkdir -p build/

.PHONY: run
run:
	docker run --rm -it -p 8080:8080 -v $(PWD):/app ggc

.PHONY: clean
clean:
	rm -rf build/*
