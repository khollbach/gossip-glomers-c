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
        // ISSUE: Memory leak, but calling json_object_put causes SIGABRT
        // main.out: ./json_object.c:273: json_object_put: Assertion
        // `jso->_ref_count > 0' failed.
        /* json_object_put(init_msg); */
        json_object_put(init_msg_reply);
    }

    while ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
        json_object* echo_msg = json_tokener_parse(input_line);
        json_object* echo_msg_reply = echo_reply(echo_msg);
        printf("%s\n", json_object_to_json_string(echo_msg_reply));
        fflush(stdout);
        // ISSUE: Memory leak, but calling json_object_put causes SIGABRT
        // main.out: ./json_object.c:273: json_object_put: Assertion
        // `jso->_ref_count > 0' failed.
        /* json_object_put(echo_msg); */
        json_object_put(echo_msg_reply);
    }

    free(input_line);
}

json_object* init_reply(json_object* init_msg)
{
    // Create `msg_id`, `dest`, `src`, `in_reply_to` fields
    json_object* reply_msg_id = json_object_new_int64(REPLY_MSG_ID++);
    json_object* reply_dest = json_object_object_get(init_msg, "src");
    json_object* reply_src = json_object_object_get(init_msg, "dest");
    json_object* reply_in_reply_to = json_object_object_get(
        json_object_object_get(init_msg, "body"), "msg_id");

    // Create `reply` and add dest`, `src` fields
    json_object* reply = json_object_new_object();
    json_object_object_add(reply, "src", reply_src);
    json_object_object_add(reply, "dest", reply_dest);

    // Create `body` field
    json_object* reply_body = json_object_new_object();
    json_object_object_add(reply_body, "type",
                           json_object_new_string("init_ok"));
    json_object_object_add(reply_body, "msg_id", reply_msg_id);
    json_object_object_add(reply_body, "in_reply_to", reply_in_reply_to);

    // Add `body` to `reply` object
    json_object_object_add(reply, "body", reply_body);

    return reply;
}

json_object* echo_reply(json_object* echo_msg)
{
    // Create `msg_id`, `dest`, `src`, `in_reply_to` fields
    json_object* reply_msg_id = json_object_new_int64(REPLY_MSG_ID++);
    json_object* reply_dest = json_object_object_get(echo_msg, "src");
    json_object* reply_src = json_object_object_get(echo_msg, "dest");
    json_object* reply_in_reply_to = json_object_object_get(
        json_object_object_get(echo_msg, "body"), "msg_id");

    // Extract `echo` field
    json_object* reply_echo = json_object_object_get(
        json_object_object_get(echo_msg, "body"), "echo");

    // Create `reply` and add dest`, `src` fields
    json_object* reply = json_object_new_object();
    json_object_object_add(reply, "src", reply_src);
    json_object_object_add(reply, "dest", reply_dest);

    // Create `body` field
    json_object* reply_body = json_object_new_object();
    json_object_object_add(reply_body, "type",
                           json_object_new_string("echo_ok"));
    json_object_object_add(reply_body, "msg_id", reply_msg_id);
    json_object_object_add(reply_body, "in_reply_to", reply_in_reply_to);

    // "Echo" the `echo` field
    json_object_object_add(reply_body, "echo", reply_echo);

    // Add `body` to `reply` object
    json_object_object_add(reply, "body", reply_body);

    return reply;
}
