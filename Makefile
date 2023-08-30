CFLAGS += $(shell pkg-config --cflags json-c)
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += $(shell pkg-config --libs json-c)
SRC_DIR := src
BUILD_DIR:= build
LIB_DIR := lib
TESTS_DIR := tests

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

# TODO(kevan): clean this up before committing.
$(BUILD_DIR)/tcp-test.out: $(BUILD_DIR)/tcp-test.o $(BUILD_DIR)/util.o $(BUILD_DIR)/tcp.o $(BUILD_DIR)/collections.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
$(BUILD_DIR)/tcp-test.o: tests/lib/tcp-test.c $(LIB_DIR)/util.h $(LIB_DIR)/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

# Generate object files for library modules
$(BUILD_DIR)/util.o: $(LIB_DIR)/util.c $(LIB_DIR)/util.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/collections.o: $(LIB_DIR)/collections.c $(LIB_DIR)/collections.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vec_deque.o: $(LIB_DIR)/vec_deque.c $(LIB_DIR)/vec_deque.h
	$(CC) $(CFLAGS) -c $< -o $@

# Tests section
UNITY_SRC := ../unity/src/unity.c
$(BUILD_DIR)/unity.o: $(UNITY_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

VEC_DEQUE_TEST_SRC := $(TESTS_DIR)/$(LIB_DIR)/vec_deque_tests.c
VEC_DEQUE_TEST_OBJS := $(BUILD_DIR)/vec_deque_tests.o $(BUILD_DIR)/vec_deque.o $(BUILD_DIR)/unity.o

$(BUILD_DIR)/vec_deque_tests.out: $(VEC_DEQUE_TEST_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/vec_deque_tests.o: $(VEC_DEQUE_TEST_SRC) $(LIB_DIR)/vec_deque.h
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/tcp.o: $(LIB_DIR)/tcp.c $(LIB_DIR)/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: tests
tests: $(BUILD_DIR)/vec_deque_tests.out
	valgrind --leak-check=full ./$(BUILD_DIR)/vec_deque_tests.out

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
