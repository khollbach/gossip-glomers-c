#include <stdint.h>
#include <stdio.h>
#include <json-c/json.h>

#include "../../lib/util.h"

json_object* init_reply(json_object* init_msg);
json_object* echo_reply(json_object* echo_msg);

int main()
{
    char* input_line = NULL;
    size_t input_len = 0;
    ssize_t input_bytes_read;

    if ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1) {
        json_object* init_msg = json_tokener_parse(input_line);
        json_object* init_msg_reply = init_reply(init_msg);
        printf("%s\n", json_object_to_json_string(init_msg_reply));
        fflush(stdout);
        json_object_put(init_msg_reply);
        json_object_put(init_msg);
    }

    while ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
        json_object* echo_msg = json_tokener_parse(input_line);
        json_object* echo_msg_reply = echo_reply(echo_msg);
        printf("%s\n", json_object_to_json_string(echo_msg_reply));
        fflush(stdout);
        json_object_put(echo_msg_reply);
        json_object_put(echo_msg);
    }

    free(input_line);
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg);
}

json_object* echo_reply(json_object* echo_msg)
{
    json_object* reply = generic_reply(echo_msg);

    // reply.body.echo = echo_msg.body.echo
    json_object* reply_echo = json_object_object_get(json_object_object_get(echo_msg, "body"), "echo");
    json_object_get(reply_echo);
    json_object_object_add(json_object_object_get(reply, "body"), "echo", reply_echo);

    return reply;
}
