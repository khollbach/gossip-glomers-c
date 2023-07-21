#pragma once

#include <stdint.h>
#include <json-c/json.h>

// "Takes ownership" of the input object (this function will call `put`).
void msg_send(json_object* msg);

// Returns an "owned" object (the caller must eventually call `put`).
//
// Returns NULL if there are no more messages.
json_object* msg_recv();

// Returns an "owned" object (the caller must eventually call `put`).
//
// Merely "borrows" the input object -- ownership stays with the caller.
json_object* generic_reply(json_object* msg);
