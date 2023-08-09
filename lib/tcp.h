#pragma once

#include <json-c/json.h>

void msg_send_pusher(json_object* msg);

json_object* msg_recv_listener();
