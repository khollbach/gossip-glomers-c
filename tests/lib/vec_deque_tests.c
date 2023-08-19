#include <stdlib.h>
#include <string.h>
#include "../../unity/src/unity.h"
#include "../../lib/vec_deque.h" // Replace with the actual name of your header file

//Convenience methods for pushing/popping ints:

static void push_back_int(VecDeque* deque, int value) {
    int* value_ptr = malloc(sizeof(int));
    *value_ptr = value;
    vec_deque_push_back(deque, value_ptr);
}

static void push_front_int(VecDeque* deque, int value) {
    int* value_ptr = malloc(sizeof(int));
    *value_ptr = value;
    vec_deque_push_front(deque, value_ptr);
}

static int pop_back_int(VecDeque* deque) {
    int* value_ptr = (int*)vec_deque_pop_back(deque);
    int value = *value_ptr;
    free(value_ptr);
    return value;
}

static int pop_front_int(VecDeque* deque) {
    int* value_ptr = (int*)vec_deque_pop_front(deque);
    int value = *value_ptr;
    free(value_ptr);
    return value;
}


void setUp(void) {}

void tearDown(void) {}

void vec_deque_test_simple_pops() {
    VecDeque* deque = vec_deque_init(3);
    TEST_ASSERT_EQUAL(0, vec_deque_length(deque));

    push_back_int(deque, 42);
    push_front_int(deque, 1337);
    
    TEST_ASSERT_EQUAL(2, vec_deque_length(deque));

    TEST_ASSERT_EQUAL(42, pop_back_int(deque));
    TEST_ASSERT_EQUAL(1337, pop_front_int(deque));
    vec_deque_free(deque);
}

void vec_deque_test_push_pop_loops() {

    VecDeque* deque = vec_deque_init(3);
    for(int i = 0; i < 10; i++) {
        push_back_int(deque, i);
        TEST_ASSERT_EQUAL(i, pop_front_int(deque));
    }
    for(int i = 0; i < 10; i++) {
        push_front_int(deque, i);
        TEST_ASSERT_EQUAL(i, pop_back_int(deque));
    }
    vec_deque_free(deque);
}

void vec_deque_test_pop_empty() {
    VecDeque* deque = vec_deque_init(3);
    TEST_ASSERT_EQUAL(NULL, vec_deque_pop_back(deque));
    vec_deque_free(deque);
}

void vec_deque_test_reserve_boundary_parts() {
    VecDeque* deque = vec_deque_init(3);
    int count = 10;

    for(int i = 1; i <= count; i++) {
        push_back_int(deque, i);
    }

    TEST_ASSERT_EQUAL(count, vec_deque_length(deque));

    for(int i = count; i >= 1; i--) {
       TEST_ASSERT_EQUAL(i, pop_back_int(deque));
       // vec_deque_int_debug_print(deque);
    }
    vec_deque_free(deque);
}

void vec_deque_test_reserve_boundary_whole() {
    
    VecDeque* deque = vec_deque_init(3);

    //change position of "cursors" so that start < end
    push_front_int(deque, 5);
    TEST_ASSERT_EQUAL(5, pop_back_int(deque));
    
    int count = 10;

    for(int i = 1; i <= count; i++) {
        push_back_int(deque, i);
    }

    TEST_ASSERT_EQUAL(count, vec_deque_length(deque));

    for(int i = count; i >= 1; i--) {
       TEST_ASSERT_EQUAL(i, pop_back_int(deque));
    }
    vec_deque_free(deque);
}

void vec_deque_test_alloc_zero() {
    VecDeque* deque = vec_deque_init(0);
    push_back_int(deque, 42);
    push_front_int(deque, 1337);
    
    TEST_ASSERT_EQUAL(2, vec_deque_length(deque));

    TEST_ASSERT_EQUAL(42, pop_back_int(deque));
    TEST_ASSERT_EQUAL(1337, pop_front_int(deque));
    vec_deque_free(deque);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(vec_deque_test_simple_pops);
    RUN_TEST(vec_deque_test_pop_empty);
    RUN_TEST(vec_deque_test_push_pop_loops);
    RUN_TEST(vec_deque_test_reserve_boundary_parts);
    RUN_TEST(vec_deque_test_reserve_boundary_whole);
    RUN_TEST(vec_deque_test_alloc_zero);
    return UNITY_END();
}
