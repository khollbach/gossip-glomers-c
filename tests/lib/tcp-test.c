#include <json-c/json.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib/tcp.h"
#include "../../lib/util.h"

void ping_random_peer(json_object* original_msg, const char** peers,
                      size_t num_peers, const char* self);
void pong(json_object* ping);
void echo(json_object* echo_request);

int main()
{
    json_object* init_msg = msg_recv();
    if (init_msg == NULL)
    {
        fprintf(stderr, "expected init message, got EOF\n");
        exit(EXIT_FAILURE);
    }

    const char** peers = node_ids(init_msg);
    size_t num_peers = node_ids_count(init_msg);
    tcp_init(peers, num_peers);
    const char* self = node_id(init_msg);
    msg_send(generic_reply(init_msg));

    json_object* msg;
    while ((msg = msg_recv_listener()) != NULL)
    {
        const char* type = msg_type(msg);
        if (strcmp(type, "echo") == 0)
        {
            ping_random_peer(msg, peers, num_peers, self);
        }
        else if (strcmp(type, "ping") == 0)
        {
            pong(msg);
        }
        else if (strcmp(type, "pong") == 0)
        {
            // Extract the original message
            json_object* body = json_object_object_get(msg, "body");
            json_object* orig = json_object_object_get(body, "original_msg");
            json_object_get(orig);
            json_object_put(msg);

            // Reply to the original client.
            echo(orig);
        }
        else
        {
            fprintf(stderr, "invalid incoming message type: %s\n", type);
            exit(EXIT_FAILURE);
        }
    }

    tcp_free();
    json_object_put(init_msg);
}

// Takes ownership of original_msg. Borrows `peers` and `self`.
void ping_random_peer(json_object* original_msg, const char** peers,
                      size_t num_peers, const char* self)
{
    json_object* ping = generic_reply(original_msg);

    // Choose a random peer (not myself)
    int i;
    if (num_peers == 1)
    {
        fprintf(stderr, "Error: ping_random_peer: only one peer\n");
        exit(EXIT_FAILURE);
    }
    do
    {
        i = rand() % num_peers;
    } while (strcmp(peers[i], self) == 0);
    json_object* dest = json_object_new_string(peers[i]);
    json_object_object_add(ping, "dest", dest);

    // type = "ping"
    json_object* body = json_object_object_get(ping, "body");
    json_object* type = json_object_new_string("ping");
    json_object_object_add(body, "type", type);

    // Include the original message as a payload.
    json_object_object_add(body, "original_msg", original_msg);

    msg_send_pusher(ping);
}

// Takes ownership of `ping`.
void pong(json_object* ping)
{
    // Create a "pong" message.
    json_object* pong = generic_reply(ping);
    json_object* pong_body = json_object_object_get(pong, "body");
    json_object* type = json_object_new_string("pong");
    json_object_object_add(pong_body, "type", type);

    // Clone the payload.
    json_object* ping_body = json_object_object_get(ping, "body");
    json_object* original_msg =
        json_object_object_get(ping_body, "original_msg");
    json_object_get(original_msg);
    json_object_put(ping);
    json_object_object_add(pong_body, "original_msg", original_msg);

    msg_send_pusher(pong);
}

// Takes ownership of `echo_request`.
void echo(json_object* echo_request)
{
    json_object* reply = generic_reply(echo_request);

    // payload = echo_request.body.echo
    json_object* req_body = json_object_object_get(echo_request, "body");
    json_object* payload = json_object_object_get(req_body, "echo");
    json_object_get(payload);
    json_object_put(echo_request);

    // reply.body.echo = payload
    json_object* reply_body = json_object_object_get(reply, "body");
    json_object_object_add(reply_body, "echo", payload);

    msg_send(reply);
}
