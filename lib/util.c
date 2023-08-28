#include "util.h"
#include "json-c/json_object.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void msg_send(json_object* msg)
{
    printf("%s\n", json_object_to_json_string(msg));
    fflush(stdout);
    json_object_put(msg);
}

json_object* msg_recv()
{
    char* input_line = NULL;
    size_t input_len = 0;

    errno = 0;
    ssize_t input_bytes_read = getline(&input_line, &input_len, stdin);
    if (input_bytes_read == -1)
    {
        // End of stdin; return NULL.
        if (errno == 0)
        {
            free(input_line);
            return NULL;
        }

        // IO error.
        perror("getline");
        free(input_line);
        exit(EXIT_FAILURE);
    }

    json_object* msg = json_tokener_parse(input_line);
    if (msg == NULL)
    {
        fprintf(stderr, "Error: recv_msg: couldn't parse line as json\n");
        free(input_line);
        exit(EXIT_FAILURE);
    }

    free(input_line);
    return msg;
}

uint64_t REPLY_MSG_ID = 0;

json_object* generic_reply(json_object* msg)
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

    json_object* reply_in_reply_to =
        json_object_object_get(json_object_object_get(msg, "body"), "msg_id");
    json_object_get(reply_in_reply_to);
    json_object_object_add(reply_body, "in_reply_to", reply_in_reply_to);

    // Generate reply type: "XXX_ok"
    const char* name = msg_type(msg);
    size_t n = strlen(name);
    char new_name[n + 4]; // +4 for "_ok" and null terminator.
    strcpy(new_name, name);
    strcpy(&(new_name[n]), "_ok");
    json_object* reply_type = json_object_new_string(new_name);
    json_object_object_add(reply_body, "type", reply_type);

    // Add a unique msg id to the reply.
    uint64_t msg_id = REPLY_MSG_ID++;
    json_object* reply_msg_id = json_object_new_int64(msg_id);
    json_object_object_add(reply_body, "msg_id", reply_msg_id);

    return reply;
}

const char* node_id(json_object* init_msg)
{
    json_object* body = json_object_object_get(init_msg, "body");
    json_object* node_id = json_object_object_get(body, "node_id");
    return json_object_get_string(node_id);
}

const char** node_ids(json_object* init_msg)
{
    json_object* body = json_object_object_get(init_msg, "body");
    json_object* node_ids = json_object_object_get(body, "node_ids");
    if (json_object_get_type(node_ids) != json_type_array)
    {
        fprintf(stderr, "Error: node_ids: node_ids is not an array\n");
        exit(EXIT_FAILURE);
    }
    size_t array_length = json_object_array_length(node_ids);
    const char** peers = malloc(sizeof(char*) * array_length);
    for (size_t i = 0; i < array_length; i++)
    {
        json_object* node_id = json_object_array_get_idx(node_ids, i);
        if (json_object_get_type(node_id) != json_type_string)
        {
            fprintf(stderr, "Error: node_ids: node_id is not a string\n");
            exit(EXIT_FAILURE);
        }
        peers[i] = json_object_get_string(node_id);
    }
    return peers;
}

size_t node_ids_count(json_object* init_msg)
{
    json_object* body = json_object_object_get(init_msg, "body");
    json_object* node_ids = json_object_object_get(body, "node_ids");
    if (json_object_get_type(node_ids) != json_type_array)
    {
        fprintf(stderr, "Error: node_ids: node_ids is not an array\n");
        exit(EXIT_FAILURE);
    }
    return json_object_array_length(node_ids);
}

const char* msg_type(json_object* msg)
{
    json_object* body = json_object_object_get(msg, "body");
    json_object* type = json_object_object_get(body, "type");
    return json_object_get_string(type);
}
