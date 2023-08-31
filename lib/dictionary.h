#pragma once

#include <stdbool.h>
#include <stddef.h>
typedef struct Dictionary Dictionary;

Dictionary* dictionary_init(void (*elem_free)(void*));
void dictionary_set(Dictionary* dictionary, const char* key, void* value);
void* dictionary_get(Dictionary* dictionary, const char* key);
bool dictionary_contains(Dictionary* dictionary, const char* key);
size_t dictionary_length(Dictionary* dictionary);
void dictionary_free(Dictionary* dictionary);
