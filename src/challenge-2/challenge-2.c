#include <json-c/json.h>
#include <stdio.h>

#include "../../lib/collections.h"
#include "../../lib/util.h"
#include "json-c/json_object.h"

json_object* init_reply(json_object* init_msg);

void client_request_handler(json_object* client_request);
void conch_response_handler(json_object* conch_response);

char* LEADER_NODE = "n0";

void client_request_handler(json_object* client_request)
{

    /* Conch request schema
    {
      "src": "n_",
      "dest": "n0",
      "body": { "type": "conch_request", "client_id": "c_" }
    }
    */
    json_object* conch_request = json_object_new_object();

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

    msg_send(conch_request);
}

void conch_response_handler(json_object* conch_response)
{
    /* Conch response schema
    {
      "src": "n0",
      "dest": "n_",
      "body": { "msg_id": _, "type": "conch_response", "conch_value": _,
    "original_client_request": _ }
    }
    */

    json_object* conch_response_body =
        json_object_object_get(conch_response, "body");
    json_object* client_response = generic_reply(
        json_object_object_get(conch_response_body, "original_client_request"));

    // id set to current conch_value
    json_object* conch_value =
        json_object_object_get(conch_response_body, "conch_value");
    json_object_get(conch_value);
    json_object_object_add(json_object_object_get(client_response, "body"),
                           "conch value", conch_value);

    // Send response message to client
    msg_send(client_response);

    // Increment conch_value
    int64_t new_conch_value = json_object_get_int64(conch_value) + 1;

    /* Conch release schema
    {
      "src": "n_",
      "dest": "n0",
      "body": { "type": "conch_release", "conch_value": _ }
    }
    */
    json_object* conch_release = json_object_new_object();

    json_object* dest = json_object_new_string(LEADER_NODE);
    json_object_object_add(conch_release, "dest", dest);

    json_object* src = json_object_object_get(conch_response, "dest");
    json_object_get(src);
    json_object_object_add(conch_release, "src", src);

    json_object* conch_release_body = json_object_new_object();
    json_object_object_add(conch_release_body, "type",
                           json_object_new_string("conch_rerelease"));
    json_object_object_add(conch_release_body, "conch_value",
                           json_object_new_int64(new_conch_value));
    json_object_object_add(conch_release, "body", conch_release_body);

    // Send release message to Leader
    msg_send(conch_release);
}

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
}

json_object* init_reply(json_object* init_msg)
{
    return generic_reply(init_msg);
}
