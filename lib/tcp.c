#include "tcp.h"
#include "collections.h"
#include "json-c/json_object.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_SEND_QUEUE_SIZE 100
#define INITIAL_SEND_MSG_SEQ_INDEX 0
#define INITIAL_RECV_MSG_SEQ_INDEX -1

typedef struct ChannelState
{
    // Send state
    Queue* send_queue;
    uint64_t next_send_msg_seq_index;
    // Receive state
    uint64_t last_recv_msg_seq_index;
} ChannelState;

typedef struct Message
{
    uint64_t msg_seq_index;
    json_object* msg;
} Message;

Dictionary* CHANNEL_STATES = NULL;

// Helper function for using `json_object`s in a `Queue`.
static void queue_json_object_free(void* obj) {
    json_object_put(obj);
}

ChannelState* channel_state_init()
{
    ChannelState* channel_state = malloc(sizeof(ChannelState));
    channel_state->send_queue = queue_init(MAX_SEND_QUEUE_SIZE, queue_json_object_free);
    channel_state->next_send_msg_seq_index = INITIAL_SEND_MSG_SEQ_INDEX;
    channel_state->last_recv_msg_seq_index = INITIAL_RECV_MSG_SEQ_INDEX;
    return channel_state;
}

void tcp_init(const char** peers, const size_t num_peers)
{
    CHANNEL_STATES = dictionary_init();
    for (size_t i = 0; i < num_peers; i++)
    {
        // fprintf(stderr, "tcp_init: before set/get for peer: %s\n", peers[i]);
        dictionary_set(CHANNEL_STATES, peers[i], channel_state_init());
        // dictionary_get(CHANNEL_STATES, peers[i]);
    }
}

void tcp_free(const char** peers, const size_t num_peers)
{
    if (CHANNEL_STATES == NULL)
    {
        fprintf(stderr, "Error: tcp_free: tcp_init not called\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_peers; i++)
    {
        ChannelState* channel_state = dictionary_get(CHANNEL_STATES, peers[i]);
        queue_free(channel_state->send_queue);
    }
    dictionary_free(CHANNEL_STATES);
}

// Borrows `msg`.
static json_object* generate_ACK_msg(json_object* msg)
{
    json_object* ack_msg = generic_reply(msg);
    json_object* body = json_object_object_get(ack_msg, "body");
    json_object_object_add(body, "type", json_object_new_string("ACK"));
    return ack_msg;
}

void msg_send_pusher(json_object* msg)
{
    const char* peer =
        json_object_get_string(json_object_object_get(msg, "dest"));
    Message* m = malloc(sizeof(Message));
    fprintf(stderr, "(push) before get. peer: %s\n", peer);
    ChannelState* channel_state = dictionary_get(CHANNEL_STATES, peer);
    fprintf(stderr, "(push) after get\n");
    m->msg_seq_index = channel_state->next_send_msg_seq_index++;
    json_object_object_add(msg, "seq_msg_index",
                           json_object_new_uint64(m->msg_seq_index));
    m->msg = msg;
    queue_enqueue(channel_state->send_queue, m);
}

json_object* msg_recv_listener()
{
    while (true)
    {
        json_object* m = msg_recv();
        if (m == NULL)
        {
            return m;
        }

        // Client messages; no need to ACK these.
        const char* src = json_object_get_string(json_object_object_get(m, "src"));
        if (!dictionary_contains(CHANNEL_STATES, src)) {
            return m;
        }

        fprintf(stderr, "(listen) before get. src: %s\n", src);
        ChannelState* channel_state = dictionary_get(CHANNEL_STATES, src);
        fprintf(stderr, "(listen) after get\n");
        json_object* body = json_object_object_get(m, "body");
        json_object* type = json_object_object_get(body, "type");

        // Case: Received an ACK
        if (strcmp(json_object_get_string(type), "ACK") == 0 &&
            !queue_is_empty(channel_state->send_queue))
        {
            uint64_t ack_seq_msg_index = json_object_get_uint64(
                json_object_object_get(body, "seq_msg_index"));
            uint64_t head_seq_msg_index =
                ((Message*)queue_peek(channel_state->send_queue))
                    ->msg_seq_index;
            if (head_seq_msg_index == ack_seq_msg_index)
            {
                free(queue_dequeue(channel_state->send_queue));
            }
        }
        // Case: Received a data message
        else
        {
            json_object* ack_msg = generate_ACK_msg(m);
            msg_send(ack_msg);

            // TODO: return the data message to the caller of this function.
        }
    }
}
