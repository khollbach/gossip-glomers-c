#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "dictionary.h"

#define INITIAL_DICTIONARY_MAX_LENGTH 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static void swap(void** a, void** b)
{
    void* temp = *a;
    *a = *b;
    *b = temp;
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
    void (*elem_free)(void*);
    size_t max_length;
    size_t length;
} Dictionary;

typedef struct DictionaryLookupResult
{
    bool found;
    size_t index;
} DictionaryLookupResult;

Dictionary* dictionary_init(void (*elem_free)(void*))
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
    dictionary->elem_free = elem_free;
    dictionary->length = 0;
    return dictionary;
}

static DictionaryLookupResult dictionary_lookup(Dictionary* dictionary,
                                                const char* key)
{
    size_t index = hash_key(key) % dictionary->max_length;
    while (dictionary->key_value_pairs[index].key != NULL)
    {
        if (strcmp(dictionary->key_value_pairs[index].key, key) == 0)
        {
            return (DictionaryLookupResult){true, index};
        }
        index = (index + 1) % dictionary->max_length;
    }
    return (DictionaryLookupResult){false, index};
}

bool dictionary_contains(Dictionary* dictionary, const char* key)
{
    DictionaryLookupResult lookup_result = dictionary_lookup(dictionary, key);
    return lookup_result.found;
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
    DictionaryLookupResult lookup_result = dictionary_lookup(dictionary, key);
    // Key does not exist, add new KeyValuePair
    if (!lookup_result.found)
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
        dictionary->key_value_pairs[lookup_result.index] = key_value_pair;
        dictionary->length++;
        dictionary_rebuild(dictionary);
    }
    // Key already exists, just replace value (no need to free the key)
    else
    {
        // FIX: Leaked underlying queue in the ChannelState value
        dictionary->elem_free(dictionary->key_value_pairs[lookup_result.index].value);
        dictionary->key_value_pairs[lookup_result.index].value = value;
        return;
    }
}

void* dictionary_get(Dictionary* dictionary, const char* key)
{
    DictionaryLookupResult lookup_result = dictionary_lookup(dictionary, key);
    // Key successfully found, return value
    if (lookup_result.found)
    {
        return dictionary->key_value_pairs[lookup_result.index].value;
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
            dictionary->elem_free(key_value_pair.value);
        }
    }
    free(dictionary->key_value_pairs);
    free(dictionary);
}
