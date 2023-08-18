#include <json-c/json.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../lib/util.h"

json_object* init_reply(json_object* init_msg);
json_object* echo_reply(json_object* echo_msg);

int main()
{
    json_object* init_msg = msg_recv();
    if (init_msg == NULL)
    {
        fprintf(stderr, "expected init message, got EOF\n");
        exit(EXIT_FAILURE);
    }
    msg_send(init_reply(init_msg));
    json_object_put(init_msg);

    json_object* echo_msg;
    while ((echo_msg = msg_recv()) != NULL)
    {
        msg_send(echo_reply(echo_msg));
        json_object_put(echo_msg);
    }
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg);
}

json_object* echo_reply(json_object* echo_msg)
{
    json_object* reply = generic_reply(echo_msg);

    // reply.body.echo = echo_msg.body.echo
    json_object* reply_echo = json_object_object_get(
        json_object_object_get(echo_msg, "body"), "echo");
    json_object_get(reply_echo);
    json_object_object_add(json_object_object_get(reply, "body"), "echo",
                           reply_echo);

    return reply;
}
