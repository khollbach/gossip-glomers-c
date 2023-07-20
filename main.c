#include <json-c/json_object.h>
#include <json-c/json_types.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <json-c/json.h>

json_object* init_reply(json_object* init_msg);
json_object* echo_reply(json_object* echo_msg);

uint64_t REPLY_MSG_ID = 0;

int main()
{
    char* input_line = NULL;
    size_t input_len = 0;
    ssize_t input_bytes_read;

    while ((REPLY_MSG_ID == 0) &&
           (input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
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

json_object* generic_reply(json_object* msg, uint64_t* next_msg_id)
{
    json_object* reply = json_object_new_object();

    json_object* reply_body = json_object_new_object();
    json_object_object_add(reply, "body", reply_body);

    // Grab shared objects, incr refcounts.

    json_object* reply_dest = json_object_object_get(msg, "src");
    json_object_get(reply_dest);
    json_object_object_add(reply, "dest", reply_dest);

    json_object* reply_src = json_object_object_get(msg, "dest");
    json_object_get(reply_src);
    json_object_object_add(reply, "src", reply_src);

    json_object* reply_in_reply_to = json_object_object_get(json_object_object_get(msg, "body"), "msg_id");
    json_object_get(reply_in_reply_to);
    json_object_object_add(reply_body, "in_reply_to", reply_in_reply_to);

    // Generate reply type: "XXX_ok"
    json_object* type = json_object_object_get(json_object_object_get(msg, "body"), "type");
    const char *name = json_object_get_string(type);
    size_t n = strlen(name);
    char new_name[n + 4]; // +4 for "_ok" and null terminator.
    strcpy(new_name, name);
    strcpy(&(new_name[n]), "_ok");
    json_object* reply_type = json_object_new_string(new_name);
    json_object_object_add(reply_body, "type", reply_type);

    // Add a unique msg id to the reply.
    uint64_t msg_id = (*next_msg_id)++;
    json_object* reply_msg_id = json_object_new_int64(msg_id);
    json_object_object_add(reply_body, "msg_id", reply_msg_id);

    return reply;
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg, &REPLY_MSG_ID);
}

json_object* echo_reply(json_object* echo_msg)
{
    json_object* reply = generic_reply(echo_msg, &REPLY_MSG_ID);

    // reply.body.echo = echo_msg.body.echo
    json_object* reply_echo = json_object_object_get(json_object_object_get(echo_msg, "body"), "echo");
    json_object_get(reply_echo);
    json_object_object_add(json_object_object_get(reply, "body"), "echo", reply_echo);

    return reply;
}
