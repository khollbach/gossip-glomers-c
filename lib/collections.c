#include <stdio.h>
#include <stdlib.h>

#include "collections.h"

bool queue_is_full(Queue* queue) { return queue->length == queue->size; }

bool queue_is_empty(Queue* queue) { return queue->length == 0; }

Queue* queue_init(size_t size)
{
    Queue* queue = malloc(sizeof(Queue));
    if (queue == NULL)
    {
        fprintf(stderr, "Error: queue_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    queue->size = size;
    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void queue_enqueue(Queue* queue, void* data)
{
    if (queue_is_full(queue))
    {
        fprintf(stderr, "Error: queue_enqueue: queue is full\n");
        exit(EXIT_FAILURE);
    }
    Node* node = malloc(sizeof(Node));
    if (node == NULL)
    {
        fprintf(stderr, "Error: queue_enqueue: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    node->next = NULL;
    if (queue_is_empty(queue))
    {
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->length++;
}

void* queue_dequeue(Queue* queue)
{
    if (queue_is_empty(queue))
    {
        fprintf(stderr, "Error: queue_dequeue: queue is empty\n");
        exit(EXIT_FAILURE);
    }
    Node* node = queue->head;
    void* data = node->data;
    queue->head = node->next;
    free(node);
    queue->length--;
    return data;
}

void queue_free(Queue* queue)
{
    while (!queue_is_empty(queue))
    {
        queue_dequeue(queue);
    }
    free(queue);
}
