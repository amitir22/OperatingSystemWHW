#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <pthread.h>
#include "message.h"
#include "logger.h"

typedef struct t_message_queue *MessageQueue;

typedef enum e_mq_return_code {
    MQ_SUCCESS,
    MQ_ERR_NULL_ARGS,
    MQ_ERR_MEMORY_FAIL,
    MQ_ERR_MISMATCH_CONTENT_TYPE,
    MQ_ERR_GENERAL_FAILURE
} MQRetCode;

MessageQueue MQCreate(unsigned int capacity, MessageContentType messageType);
void MQFree(MessageQueue messageQueue);
MQRetCode MQPut(MessageQueue messageQueue, Message message);
MQRetCode MQGet(MessageQueue messageQueue, Message *message);
unsigned int MQGetSize(MessageQueue messageQueue);
unsigned int MQGetCapacity(MessageQueue messageQueue);

#endif //MESSAGE_QUEUE_H
