#include <stdbool.h>
#include <stdio.h>
#include<stdlib.h>

#include "conch.h"

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



