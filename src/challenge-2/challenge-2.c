#include <json-c/json.h>
#include <stdio.h>

#include "../../lib/collections.h"
#include "../../lib/util.h"

json_object* init_reply(json_object* init_msg);

int main(void)
{
    char* input_line = NULL;
    size_t input_len = 0;
    ssize_t input_bytes_read;

    if ((input_bytes_read = getline(&input_line, &input_len, stdin)) != -1)
    {
        json_object* init_msg = json_tokener_parse(input_line);
        json_object* init_msg_reply = init_reply(init_msg);
        printf("%s\n", json_object_to_json_string(init_msg_reply));
        fflush(stdout);
        json_object_put(init_msg_reply);
        json_object_put(init_msg);
    }

    // NOTE: This is purely for testing; will be removed in final version
    Queue* queue = queue_init(10);
    for (int i = 0; i < 10; i++)
    {
        int* data = malloc(sizeof(int));
        *data = i;
        queue_enqueue(queue, data);
    }
    for (int i = 0; i < 10; i++)
    {
        int* data = queue_dequeue(queue);
        printf("%d\n", *data);
        free(data);
    }
    queue_free(queue);

    free(input_line);
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg);
}
