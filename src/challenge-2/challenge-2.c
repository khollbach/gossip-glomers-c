#include "../../lib/collections.h"
#include "../../lib/util.h"
#include "json-c/json_object.h"
#include <json-c/json.h>
#include <stdio.h>

char* LEADER_NODE = "n0";

json_object* init_reply(json_object* init_msg);

void client_request_handler(json_object* client_request);
void conch_response_handler(json_object* conch_response);
void conch_request_handler(json_object* conch_request, Queue* conch_queue);
void conch_release_handler(json_object* conch_release, Conch* conch);
void conch_response_dispatcher(Conch* conch, Queue* request_queue);

int main(void)
{
    json_object* init_msg = msg_recv();
    if (init_msg == NULL)
    {
        fprintf(stderr, "expected init message, got EOF");
        exit(EXIT_FAILURE);
    }
    msg_send(init_reply(init_msg));
    json_object_put(init_msg);

    // TODO: Event loops
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg);
}

// Used exclusively by Leader
void conch_request_handler(json_object* conch_request, Queue* request_queue)
{
    if (queue_is_full(request_queue))
    {
        fprintf(stderr,
                "Error: conch_request_handler: request queue is full\n");
        exit(EXIT_FAILURE);
    }
    json_object* body = json_object_object_get(conch_request, "body");
    json_object* original_client_request =
        json_object_object_get(body, "original_client_request");
    queue_enqueue(request_queue, original_client_request);
    json_object_get(original_client_request);
}

// Used exclusively by Leader
void conch_response_dispatcher(Conch* conch, Queue* request_queue)
{
    if (!conch_is_available(conch))
    {
        fprintf(stderr,
                "Error: conch_response_dispatcher: conch is not available\n");
        exit(EXIT_FAILURE);
    }
    if (queue_is_empty(request_queue))
    {
        fprintf(stderr,
                "Error: conch_response_dispatcher: request queue is empty\n");
        exit(EXIT_FAILURE);
    }
    json_object* conch_response = json_object_new_object();
    json_object* original_client_request = queue_dequeue(request_queue);

    // Construct conch response message
    json_object* src = json_object_new_string(LEADER_NODE);
    json_object_object_add(conch_response, "src", src);

    json_object* dest = json_object_object_get(original_client_request, "dest");
    json_object_get(dest);
    json_object_object_add(conch_response, "dest", dest);

    json_object* body = json_object_new_object();
    json_object_object_add(body, "type",
                           json_object_new_string("conch_response"));
    int64_t conch_value = conch_checkout(conch);
    json_object_object_add(body, "conch_value",
                           json_object_new_int64(conch_value));
    json_object_object_add(body, "original_client_request",
                           original_client_request);
    json_object_object_add(conch_response, "body", body);

    // Send conch response message (to Follower)
    msg_send(conch_response);
}

// Used exclusively by Leader
void conch_release_handler(json_object* conch_release, Conch* conch)
{
    json_object* body = json_object_object_get(conch_release, "body");
    int64_t new_conch_value =
        json_object_get_int64(json_object_object_get(body, "conch_value"));
    conch_checkin(conch, new_conch_value);
}

void client_request_handler(json_object* client_request)
{
    json_object* conch_request = json_object_new_object();

    // Construct conch request message
    json_object* dest = json_object_new_string(LEADER_NODE);
    json_object_object_add(conch_request, "dest", dest);

    json_object* src = json_object_object_get(client_request, "dest");
    json_object_get(src);
    json_object_object_add(conch_request, "src", src);

    json_object* body = json_object_new_object();
    json_object_object_add(body, "type",
                           json_object_new_string("conch_request"));
    json_object_get(client_request);
    json_object_object_add(body, "original_client_request", client_request);
    json_object_object_add(conch_request, "body", body);

    // Send conch request message (to Leader)
    msg_send(conch_request);
}

void conch_response_handler(json_object* conch_response)
{
    // Construct client response message
    json_object* conch_response_body =
        json_object_object_get(conch_response, "body");
    json_object* client_response = generic_reply(
        json_object_object_get(conch_response_body, "original_client_request"));

    json_object* conch_value =
        json_object_object_get(conch_response_body, "conch_value");
    json_object_get(conch_value);
    json_object_object_add(json_object_object_get(client_response, "body"),
                           "conch_value", conch_value);

    // Send client response message (to client)
    msg_send(client_response);

    // Increment conch_value
    int64_t new_conch_value = json_object_get_int64(conch_value) + 1;

    // Construct conch release message
    json_object* conch_release = json_object_new_object();

    json_object* dest = json_object_new_string(LEADER_NODE);
    json_object_object_add(conch_release, "dest", dest);

    json_object* src = json_object_object_get(conch_response, "dest");
    json_object_get(src);
    json_object_object_add(conch_release, "src", src);

    json_object* conch_release_body = json_object_new_object();
    json_object_object_add(conch_release_body, "type",
                           json_object_new_string("conch_release"));
    json_object_object_add(conch_release_body, "conch_value",
                           json_object_new_int64(new_conch_value));
    json_object_object_add(conch_release, "body", conch_release_body);

    // Send conch release message (to Leader)
    msg_send(conch_release);
}
