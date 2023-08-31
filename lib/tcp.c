#include "tcp.h"
#include "collections.h"
#include "json-c/json_object.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SEND_QUEUE_SIZE 100
#define INITIAL_SEND_MSG_SEQ_INDEX 0
#define INITIAL_RECV_MSG_SEQ_INDEX 0

typedef struct ChannelState
{
    // Send state
    Queue* send_queue;
    uint64_t next_send_msg_seq_index;
    // Receive state
    uint64_t expected_recv_msg_seq_index;
} ChannelState;

typedef struct Message
{
    uint64_t msg_seq_index;
    json_object* msg;
} Message;

Dictionary* CHANNEL_STATES = NULL;
char** PEERS = NULL;
size_t NUM_PEERS = 0;

// Helper function for using `json_object`s in a `Queue`.
static void queue_json_object_free(void* obj) { json_object_put(obj); }

ChannelState* channel_state_init()
{
    ChannelState* channel_state = malloc(sizeof(ChannelState));
    channel_state->send_queue =
        queue_init(MAX_SEND_QUEUE_SIZE, queue_json_object_free);
    channel_state->next_send_msg_seq_index = INITIAL_SEND_MSG_SEQ_INDEX;
    channel_state->expected_recv_msg_seq_index = INITIAL_RECV_MSG_SEQ_INDEX;
    return channel_state;
}

void channel_state_free(ChannelState* channel_state)
{
    queue_free(channel_state->send_queue);
    free(channel_state);
}

void channel_state_free_void(void* channel_state) 
{
    channel_state_free(channel_state);
}

void tcp_init(const char** peers, const size_t num_peers)
{
    CHANNEL_STATES = dictionary_init(channel_state_free_void);
    PEERS = malloc(num_peers * sizeof(char*));
    for (size_t i = 0; i < num_peers; i++)
    {
        dictionary_set(CHANNEL_STATES, peers[i], channel_state_init());
        PEERS[i] = strdup(peers[i]);
        NUM_PEERS++;
    }
}

void tcp_free(void)
{
    if (CHANNEL_STATES == NULL)
    {
        fprintf(stderr, "Error: tcp_free: tcp_init not called\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < NUM_PEERS; i++)
    {
        ChannelState* channel_state = dictionary_get(CHANNEL_STATES, PEERS[i]);
        queue_free(channel_state->send_queue);
        free(PEERS[i]);
    }
    free(PEERS);
    dictionary_free(CHANNEL_STATES);
}

// Borrows `msg`.
static json_object* generate_ACK_msg(json_object* msg)
{
    json_object* ack_msg = generic_reply(msg);
    json_object* ack_body = json_object_object_get(ack_msg, "body");
    json_object_object_add(ack_body, "type", json_object_new_string("ACK"));
    json_object* body = json_object_object_get(msg, "body");
    json_object* seq_msg_index = json_object_object_get(body, "seq_msg_index");
    json_object_get(seq_msg_index);
    json_object_object_add(ack_body, "seq_msg_index", seq_msg_index);
    return ack_msg;
}

void msg_send_pusher(json_object* msg)
{
    fprintf(stderr, "(msg_send_pusher) start\n");
    const char* peer =
        json_object_get_string(json_object_object_get(msg, "dest"));
    Message* m = malloc(sizeof(Message));
    fprintf(stderr, "(push) before get. peer: %s\n", peer);
    ChannelState* channel_state = dictionary_get(CHANNEL_STATES, peer);
    fprintf(stderr, "(push) after get\n");
    m->msg_seq_index = channel_state->next_send_msg_seq_index++;
    json_object* body = json_object_object_get(msg, "body");
    json_object_object_add(body, "seq_msg_index",
                           json_object_new_uint64(m->msg_seq_index));
    m->msg = msg;
    queue_enqueue(channel_state->send_queue, m);
}

json_object* msg_recv_listener()
{
    while (true)
    {
        for (size_t i = 0; i < NUM_PEERS; i++)
        {
            ChannelState* channel_state =
                dictionary_get(CHANNEL_STATES, PEERS[i]);
            if (!queue_is_empty(channel_state->send_queue))
            {
                Message* m = queue_peek(channel_state->send_queue);
                json_object_get(m->msg);
                fprintf(stderr, "(send) msg: %s\n",
                        json_object_to_json_string(m->msg));
                msg_send(m->msg);
            }
        }

        json_object* m = msg_recv();
        if (m == NULL)
        {
            return m;
        }

        // Client messages; no need to ACK these.
        const char* src =
            json_object_get_string(json_object_object_get(m, "src"));
        fprintf(stderr, "(listen) src: %s\n", src);
        if (!dictionary_contains(CHANNEL_STATES, src))
        {
            return m;
        }

        fprintf(stderr, "(listen) before get. src: %s\n", src);
        ChannelState* channel_state = dictionary_get(CHANNEL_STATES, src);
        fprintf(stderr, "(listen) after get\n");
        json_object* body = json_object_object_get(m, "body");
        json_object* type = json_object_object_get(body, "type");

        // Case: Received an ACK
        if (strcmp(json_object_get_string(type), "ACK") == 0)
        {
            if (queue_is_empty(channel_state->send_queue))
            {
                continue;
            }
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
            uint64_t data_seq_msg_index = json_object_get_uint64(
                json_object_object_get(body, "seq_msg_index"));
            if (channel_state->expected_recv_msg_seq_index ==
                data_seq_msg_index)
            {
                channel_state->expected_recv_msg_seq_index++;
                return m;
            }
        }
    }
}
