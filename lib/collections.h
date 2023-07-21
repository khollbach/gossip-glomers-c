#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Node
{
    void* data;
    struct Node* next;
} Node;

typedef struct Queue
{
    Node* head;
    Node* tail;
    size_t length;
    size_t size;
} Queue;

Queue* queue_init(size_t size);
void queue_enqueue(Queue* queue, void* data);
void* queue_dequeue(Queue* queue);
bool queue_is_full(Queue* queue);
bool queue_is_empty(Queue* queue);
void queue_free(Queue* queue);
