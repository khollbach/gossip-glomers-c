#pragma once

#include <json-c/json.h>

// Takes ownership of `msg`.
void msg_send_pusher(json_object* msg);

// Returns an owned object (or NULL).
json_object* msg_recv_listener();

// Borrows `peers`.
void tcp_init(const char** peers, const size_t num_peers);

void tcp_free(void);
