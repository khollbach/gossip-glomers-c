#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <json-c/json.h>

json_object* init_reply(json_object* init_msg);

// ISSUE: Possible memory leaks from not freeing json objects?
int main()
{
    char* input_line = NULL;
    size_t input_len = 0;
    ssize_t input_bytes_read;

    while ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
        char* json_string = input_line;
        json_object* init_msg = json_tokener_parse(json_string);
        json_object* init_msg_reply = init_reply(init_msg);
        printf("%s\n", json_object_to_json_string(init_msg_reply));
        json_object_put(init_msg);
        json_object_put(init_msg_reply);
    }

    while ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
        char* json_string = input_line;
        json_object* jobj = json_tokener_parse(json_string);
        const char* parsed_string = json_object_to_json_string(jobj);
        printf("jobj: %s\n", parsed_string);
        json_object_put(jobj);
    }
    free(input_line);
}

// ISSUE: Possible memory leaks from not freeing json objects?
json_object* init_reply(json_object* init_msg)
{
    // Create `msg_id`, `dest`, `src`, `in_reply_to` fields
    json_object* reply_msg_id = json_object_new_int64(1);
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
    json_object_object_add(reply_body, "in_reply_to", reply_in_reply_to);
    json_object_object_add(reply_body, "msg_id", reply_msg_id);
    json_object_object_add(reply_body, "type",
                           json_object_new_string("init_ok"));

    // Add `body` to `reply` object
    json_object_object_add(reply, "body", reply_body);

    return reply;
}
