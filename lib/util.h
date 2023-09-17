#pragma once

#include <json-c/json.h>
#include <stdint.h>

typedef enum INPUT_READ_RESULTS : long long
{
    INPUT_READ_TIMEDOUT = -1LL,
    INPUT_READ_EOF = -2LL,
} INPUT_READ_RESULTS;

// "Takes ownership" of the input object (this function will call `put`).
void msg_send(json_object* msg);

// Returns an "owned" object (the caller must eventually call `put`).
// Returns NULL if there are no more messages.
json_object* msg_recv();

// Returns an "owned" object (the caller must eventually call `put`).
// Merely "borrows" the input object -- ownership stays with the caller.
json_object* generic_reply(json_object* msg);

// Borrows init_msg.
// The returned string is borrowed from init_msg; don't use it after freeing
// init_msg.
const char* node_id(json_object* init_msg);

// Borrows init_msg.
// The returned array is owned by the caller, and must be freed.
// The strings in the array are borrowed from init_msg; do not use them after
// freeing init_msg.
const char** node_ids(json_object* init_msg);

// Borrows init_msg.
size_t node_ids_count(json_object* init_msg);

// Borrows msg.
// The returned string is borrowed from msg; don't use it after freeing msg.
const char* msg_type(json_object* msg);
