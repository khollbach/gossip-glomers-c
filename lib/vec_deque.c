#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vec_deque.h"

typedef struct VecDeque {
  size_t capacity;
  void** array;
  size_t start;
  size_t end;
  size_t len;
} Node;

static void rot_left(size_t* v, size_t cap) {
  *v  = (cap + *v - 1) % cap;
}

static void rot_right(size_t* v, size_t cap) {
  *v = (*v + 1) % cap;
}

static void null_alloc_check(void* ptr) {
  if (ptr  == NULL) {
    fprintf(stderr, "VecDeque failed alloc, out of memory?\n");
    exit(EXIT_FAILURE);
  }
}

static void resize_if_needed(VecDeque* deque) {
  if (deque->capacity == deque->len) {
    if (deque->capacity == 0) {
      deque->capacity = 1;
      deque->array = malloc(sizeof(void*));
      null_alloc_check(deque->array);
      deque->end = 0;
    } else {
      vec_deque_reserve(deque, deque->capacity);
    }
  }
}


VecDeque* vec_deque_init(size_t capacity) {
  VecDeque* deque = malloc(sizeof(VecDeque));
  null_alloc_check(deque);
  deque->capacity = capacity;
  if (capacity != 0) {
    deque->array = malloc(capacity * sizeof(void*));
    null_alloc_check(deque->array);
    deque->end = capacity - 1;
  } else {
    deque->array = NULL;
  }
  deque->start = 0; 
  deque->len = 0;
  return deque;
}

void vec_deque_push_back(VecDeque* deque, void* elem) {
  resize_if_needed(deque);
  rot_left(&deque->start, deque->capacity);
  deque->array[deque->start] = elem;
  deque->len++;
}

void vec_deque_push_front(VecDeque* deque, void* elem){
  resize_if_needed(deque);
  rot_right(&deque->end, deque->capacity);
  deque->array[deque->end] = elem;
  deque->len++;
}

void* vec_deque_pop_back(VecDeque* deque){
  if (deque->len == 0) {
    return NULL;
  }
  void* elem = deque->array[deque->start];
  rot_right(&deque->start, deque->capacity);
  deque->len--;
  return elem;
}

void* vec_deque_pop_front(VecDeque* deque){
  if (deque->len == 0) {
    return NULL;
  }
  void* elem = deque->array[deque->end];
  rot_left(&deque->end, deque->capacity);
  deque->len--;
  return elem;
}

size_t vec_deque_len(VecDeque* deque) {
  return deque->len;
}

void vec_deque_free(VecDeque* deque) {
  free(deque->array);
  free(deque);
}

void vec_deque_reserve(VecDeque* deque, size_t additional) {
  if (additional == 0) {
    return;
  }
  if (deque->capacity == 0) {
    resize_if_needed(deque);
    additional--;
    if (additional == 0) {
      return;
    }
  }
  size_t new_capacity = deque->capacity + additional;
  void** new_arr = realloc(deque->array, new_capacity * sizeof(void*));
  null_alloc_check(new_arr);
  deque->array = new_arr;

  if (deque->start <= deque->end) {
    deque->capacity = new_capacity;
    return;
  }

  size_t new_start = new_capacity - deque->capacity + deque->start;
  memmove(
    &deque->array[new_start],
    &deque->array[deque->start],
    (new_capacity - new_start) * sizeof(void*)
  );
  deque->start = new_start;
  deque->capacity = new_capacity;
}