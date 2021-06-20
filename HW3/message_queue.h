#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <pthread.h>
#include "message.h"
#include "logger.h"

typedef struct t_message_queue *MessageQueue;

typedef enum e_mq_return_code {
    MQ_SUCCESS,
    MQ_DROP,
    MQ_ERR_NULL_ARGS,
    MQ_ERR_MEMORY_FAIL
} MQRetCode;

typedef enum e_mq_sched_policy {
    INVALID,
    BLOCK,
    DROP_TAIL,
    DROP_HEAD,
    DROP_RANDOM
} MQSchedPolicy;

MessageQueue MQCreate(int capacity, char *schedAlgo);
void MQFree(MessageQueue messageQueue);
MQRetCode MQPut(MessageQueue messageQueue, Message message, Content *dropped, int *droppedAmount);
MQRetCode MQGet(MessageQueue messageQueue, Message *message);
int MQGetSize(MessageQueue messageQueue);
int MQGetCapacity(MessageQueue messageQueue);

#endif //MESSAGE_QUEUE_H
