#pragma once

#include <stddef.h>

typedef struct Queue Queue;

// Returns an owned `Queue`.
Queue* queue_init(size_t max_length, void (*free_function)(void*));

// Borrows `queue`. Takes ownership of `data`.
void queue_enqueue(Queue* queue, void* data);

// Borrows `queue`. Returns an owned pointer.
void* queue_dequeue(Queue* queue);

// Borrows `queue`. Returned pointer is borrowed from `queue`.
void* queue_peek(Queue* queue);

// Borrows `queue`.
bool queue_is_full(Queue* queue);

// Borrows `queue`.
bool queue_is_empty(Queue* queue);

// Takes ownership of `queue`.
void queue_free(Queue* queue);

