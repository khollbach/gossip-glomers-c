#include "tcp.h"
#include "collections.h"
#include "json-c/json_object.h"
#include "util.h"
#include <stdint.h>
#include <string.h>

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

ChannelState CHANNEL_STATE = {NULL, 0, -1};

void msg_send_pusher(json_object* msg)
{
    if (CHANNEL_STATE.send_queue == NULL)
    {
        CHANNEL_STATE.send_queue = queue_init(100);
    }
    Message* m = malloc(sizeof(Message));
    m->msg_seq_index = CHANNEL_STATE.next_send_msg_seq_index++;
    json_object_object_add(msg, "seq_msg_index",
                           json_object_new_uint64(m->msg_seq_index));
    m->msg = msg;
    queue_enqueue(CHANNEL_STATE.send_queue, m);
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
        // TODO: decide if m is a data message
        json_object* body = json_object_object_get(m, "body");
        json_object* type = json_object_object_get(body, "type");
        if (strcmp(json_object_get_string(type), "ACK") == 0 &&
            !queue_is_empty(CHANNEL_STATE.send_queue))
        {
            uint64_t ack_seq_msg_index = json_object_get_uint64(
                json_object_object_get(body, "seq_msg_index"));
            uint64_t head_seq_msg_index =
                ((Message*)queue_peek(CHANNEL_STATE.send_queue))->msg_seq_index;
            if (head_seq_msg_index == ack_seq_msg_index)
            {
                free(queue_dequeue(CHANNEL_STATE.send_queue));
            }
        }
        else
        {
            // TODO: Send ACK message back to R
            json_object* ack_msg = generate_ACK_msg(m);
            msg_send(ack_msg);
        }
    }
}
