#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Queue Queue;
typedef struct Conch Conch;
typedef struct List List;
typedef struct Dictionary Dictionary;

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

List* list_init(void);
void list_append(List* list, void* data);
void* list_pop(List* list);
void* list_get_at(List* list, size_t index);
void list_set_at(List* list, size_t index, void* data);
size_t list_length(List* list);
void list_free(List* list);

Dictionary* dictionary_init(void);
void dictionary_set(Dictionary* dictionary, const char* key, void* value);
void* dictionary_get(Dictionary* dictionary, const char* key);
bool dictionary_contains(Dictionary* dictionary, const char* key);
size_t dictionary_length(Dictionary* dictionary);
void dictionary_free(Dictionary* dictionary);
