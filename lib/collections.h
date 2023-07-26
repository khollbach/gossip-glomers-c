#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Queue Queue;
typedef struct Conch Conch;

Queue* queue_init(size_t size);
void queue_enqueue(Queue* queue, void* data);
void* queue_dequeue(Queue* queue);
bool queue_is_full(Queue* queue);
bool queue_is_empty(Queue* queue);
void queue_free(Queue* queue);

Conch* conch_init(int64_t initial_conch_value);
void conch_checkin(Conch* conch, int64_t new_conch_value);
int64_t conch_checkout(Conch* conch);
bool conch_is_available(Conch* conch);
void conch_free(Conch* conch);
