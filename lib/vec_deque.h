#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct VecDeque VecDeque;

VecDeque* vec_deque_init(size_t size);
void vec_deque_push_back(VecDeque* deque, void* elem);
void vec_deque_push_front(VecDeque* deque, void* elem);
void* vec_deque_pop_back(VecDeque* deque);
void* vec_deque_pop_front(VecDeque* deque);
size_t vec_deque_length(VecDeque* deque);
void vec_deque_reserve(VecDeque* deque, size_t additional);
void vec_deque_free(VecDeque* deque);
