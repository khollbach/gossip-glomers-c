#include <stdio.h>
#include <stdlib.h>

#include "collections.h"

#define INITIAL_LIST_MAX_LENGTH 16

typedef struct Node
{
    void* data;
    struct Node* next;
} Node;

// Queue ADT
typedef struct Queue
{
    Node* head;
    Node* tail;
    size_t length;
    size_t max_length;
} Queue;

bool queue_is_full(Queue* queue) { return queue->length == queue->max_length; }

bool queue_is_empty(Queue* queue) { return queue->length == 0; }

Queue* queue_init(size_t max_length)
{
    Queue* queue = malloc(sizeof(Queue));
    if (queue == NULL)
    {
        fprintf(stderr, "Error: queue_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    queue->max_length = max_length;
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

// Conch ADT
typedef struct Conch
{
    int64_t conch_value;
    bool conch_available;
} Conch;

Conch* conch_init(int64_t initial_conch_value)
{
    Conch* conch = malloc(sizeof(Conch));
    if (conch == NULL)
    {
        fprintf(stderr, "Error: conch_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    conch->conch_value = initial_conch_value;
    conch->conch_available = true;
    return conch;
}

void conch_checkin(Conch* conch, int64_t new_conch_value)
{
    conch->conch_value = new_conch_value;
    conch->conch_available = true;
}

int64_t conch_checkout(Conch* conch)
{
    if (!conch_is_available(conch))
    {
        fprintf(stderr, "Error: conch_checkout: conch is not available\n");
        exit(EXIT_FAILURE);
    }
    conch->conch_available = false;
    return conch->conch_value;
}

bool conch_is_available(Conch* conch) { return conch->conch_available; }

void conch_free(Conch* conch) { free(conch); }

// Dynamic Array ADT
typedef struct List
{
    size_t length;
    size_t max_length;
    void** data;
} List;

List* list_init(void)
{
    List* list = malloc(sizeof(List));
    if (list == NULL)
    {
        fprintf(stderr, "Error: list_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    list->length = 0;
    list->max_length = INITIAL_LIST_MAX_LENGTH;
    list->data = malloc(INITIAL_LIST_MAX_LENGTH * sizeof(void*));
    if (list->data == NULL)
    {
        fprintf(stderr, "Error: list_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return list;
}

void* list_get_at(List* list, size_t index)
{
    if (index >= list->length)
    {
        fprintf(stderr, "Error: list_get_at: index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return list->data[index];
}

void list_set_at(List* list, size_t index, void* item)
{
    if (index >= list->length)
    {
        fprintf(stderr, "Error: list_set_at: index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    list->data[index] = item;
}

static void list_resize(List* list)
{
    if (list->length == list->max_length)
    {
        list->max_length *= 2;
        list->data = realloc(list->data, list->max_length * sizeof(void*));
        if (list->data == NULL)
        {
            fprintf(stderr, "Error: list_resize: realloc failed\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (list->length * 4 <= list->max_length &&
             list->max_length > INITIAL_LIST_MAX_LENGTH)
    {
        list->max_length /= 2;
        list->data = realloc(list->data, list->max_length * sizeof(void*));
        if (list->data == NULL)
        {
            fprintf(stderr, "Error: list_resize: realloc failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

void list_append(List* list, void* item)
{
    list->length += 1;
    list_resize(list);
    list->data[list->length - 1] = item;
}

void* list_pop(List* list)
{
    if (list->length == 0)
    {
        fprintf(stderr, "Error: list_pop: list is empty\n");
        exit(EXIT_FAILURE);
    }
    void* returned_item = list->data[--list->length];
    list_resize(list);
    return returned_item;
}

size_t list_length(List* list) { return list->length; }

void list_free(List* list)
{
    free(list->data);
    free(list);
}
