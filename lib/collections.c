#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "collections.h"

#define INITIAL_LIST_MAX_LENGTH 16
#define INITIAL_DICTIONARY_MAX_LENGTH 16
#define KEY_NOT_FOUND -1
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

void swap(void** a, void** b)
{
    void* temp = *a;
    *a = *b;
    *b = temp;
}

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

void* queue_peek(Queue* queue)
{
    if (!queue_is_empty(queue))
    {
        return queue->head->data;
    }
    fprintf(stderr, "Error: queue_peek: queue is empty\n");
    exit(EXIT_FAILURE);
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
        void* data = queue_dequeue(queue);
        free(data);
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

// NOTE: This dictionary implementation assumes ownership of the values and
// creates copies of the key strings. User-inserted KeyValuePairs guarantee
// non-nullity for both key and value.
typedef struct KeyValuePair
{
    const char* key;
    void* value;
} KeyValuePair;

// FNV-1a hash function
static uint64_t hash_key(const char* key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char* c = key; *c != '\0'; c++)
    {
        hash ^= (uint64_t)(unsigned char)(*c);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Dictionary ADT
typedef struct Dictionary
{
    KeyValuePair* key_value_pairs;
    size_t max_length;
    size_t length;
} Dictionary;

Dictionary* dictionary_init(void)
{
    Dictionary* dictionary = malloc(sizeof(Dictionary));
    if (dictionary == NULL)
    {
        fprintf(stderr, "Error: dictionary_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    dictionary->key_value_pairs =
        calloc(INITIAL_DICTIONARY_MAX_LENGTH, sizeof(KeyValuePair));
    if (dictionary->key_value_pairs == NULL)
    {
        fprintf(stderr, "Error: dictionary_init: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    dictionary->max_length = INITIAL_DICTIONARY_MAX_LENGTH;
    dictionary->length = 0;
    return dictionary;
}

static uint64_t dictionary_lookup(Dictionary* dictionary, const char* key)
{
    uint64_t index = hash_key(key) % dictionary->max_length;
    while (dictionary->key_value_pairs[index].key != NULL)
    {
        if (strcmp(dictionary->key_value_pairs[index].key, key) == 0)
        {
            return index;
        }
        index = (index + 1) % dictionary->max_length;
    }
    return KEY_NOT_FOUND;
}

bool dictionary_contains(Dictionary* dictionary, const char* key)
{
    return dictionary_lookup(dictionary, key) != KEY_NOT_FOUND;
}

static void dictionary_rebuild(Dictionary* dictionary)
{
    size_t new_max_length;
    if (dictionary->length >= dictionary->max_length / 2)
    {
        new_max_length = dictionary->max_length * 2;
    }
    else
    {
        return;
    }

    Dictionary* temp_dictionary = malloc(sizeof(Dictionary));
    if (temp_dictionary == NULL)
    {
        fprintf(stderr, "Error: dictionary_rebuild: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    KeyValuePair* new_key_value_pairs =
        calloc(new_max_length, sizeof(KeyValuePair));
    if (new_key_value_pairs == NULL)
    {
        fprintf(stderr, "Error: dictionary_rebuild: calloc failed\n");
        exit(EXIT_FAILURE);
    }
    temp_dictionary->max_length = new_max_length;
    temp_dictionary->key_value_pairs = new_key_value_pairs;
    for (size_t i = 0; i < dictionary->max_length; i++)
    {
        KeyValuePair key_value_pair = dictionary->key_value_pairs[i];
        dictionary_set(temp_dictionary, key_value_pair.key,
                       key_value_pair.value);
    }

    swap((void**)&dictionary->key_value_pairs,
         (void**)&temp_dictionary->key_value_pairs);
    swap((void**)&dictionary->max_length, (void**)&temp_dictionary->max_length);

    dictionary_free(temp_dictionary);
    dictionary->max_length = temp_dictionary->max_length;
    dictionary->key_value_pairs = temp_dictionary->key_value_pairs;
}

void dictionary_set(Dictionary* dictionary, const char* key, void* value)
{
    uint64_t index = dictionary_lookup(dictionary, key);
    // Key does not exist, add new KeyValuePair
    if (index == KEY_NOT_FOUND)
    {
        char* copied_key = key ? strdup(key) : NULL;
        if (copied_key == NULL)
        {
            fprintf(stderr,
                    "Error: dictionary_set: key is NULL or strdup failed\n");
            exit(EXIT_FAILURE);
        }
        if (value == NULL)
        {
            fprintf(stderr, "Error: dictionary_set: value is NULL\n");
            exit(EXIT_FAILURE);
        }
        KeyValuePair key_value_pair = {copied_key, value};
        dictionary_rebuild(dictionary);
        index = dictionary_lookup(dictionary, key);
        dictionary->key_value_pairs[index] = key_value_pair;
        dictionary->length++;
    }
    // Key already exists, just replace value (no need to free the key)
    else
    {
        free(dictionary->key_value_pairs[index].value);
        dictionary->key_value_pairs[index].value = value;
        return;
    }
}

void* dictionary_get(Dictionary* dictionary, const char* key)
{
    uint64_t index = dictionary_lookup(dictionary, key);
    // Key successfully found, return value
    if (index != KEY_NOT_FOUND)
    {
        return dictionary->key_value_pairs[index].value;
    }
    // Raise error if key not found
    else
    {
        fprintf(stderr, "Error: dictionary_get: key not found\n");
        exit(EXIT_FAILURE);
    }
}

size_t dictionary_length(Dictionary* dictionary) { return dictionary->length; }

void dictionary_free(Dictionary* dictionary)
{
    for (size_t i = 0; i < dictionary->max_length; i++)
    {
        KeyValuePair key_value_pair = dictionary->key_value_pairs[i];
        if (key_value_pair.key != NULL)
        {
            free((void*)key_value_pair.key);
        }
        if (key_value_pair.value != NULL)
        {
            free(key_value_pair.value);
        }
    }
    free(dictionary->key_value_pairs);
    free(dictionary);
}
