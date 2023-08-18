#pragma once

#include <json-c/json.h>

void msg_send_pusher(json_object* msg);

json_object* msg_recv_listener();

void tcp_init(const char** peers, const size_t num_peers);
void tcp_free(const char** peers, const size_t num_peers);
