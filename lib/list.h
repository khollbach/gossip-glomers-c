#pragma once

#include <stddef.h>

typedef struct List List;

List* list_init(void);
void list_append(List* list, void* data);
void* list_pop(List* list);
void* list_get_at(List* list, size_t index);
void list_set_at(List* list, size_t index, void* data);
size_t list_length(List* list);
void list_free(List* list);
