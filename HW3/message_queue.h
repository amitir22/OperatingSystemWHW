#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "message.h"

typedef struct t_message_queue *MessageQueue;

typedef enum e_mq_return_code MQRetCode {
    MQ_SUCCESS,
    MQ_ERR_GENERAL_FAILURE,
    MQ_ERR_NULL_ARGS,
    MQ_ERR_FULL_QUEUE,
    MQ_ERR_EMPTY_QUEUE,
    MQ_ERR_MEMORY_FAIL
};

MessageQueue MQCreate(unsigned int capacity);
void MQFree(MessageQueue messageQueue);
MQRetCode MQPut(MessageQueue messageQueue, Message message);
MQRetCode MQGet(MessageQueue messageQueue, Message &message);
unsigned int MQGetSize(MessageQueue messageQueue);
unsigned int MQGetCapacity(MessageQueue messageQueue);

#endif //MESSAGE_QUEUE_H
