#include <stdbool.h>
#include <stdio.h>
#include<stdlib.h>

#include "list.h"


#define INITIAL_LIST_MAX_LENGTH 16

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
