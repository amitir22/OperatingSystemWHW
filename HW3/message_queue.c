#include "message_queue.h"

typedef struct t_node {
    struct t_node *next;
    Message message;
} *Node;

struct t_message_queue {
    unsigned int capacity;
    unsigned int size;
    Node head;
    Node tail;

    pthread_cond_t cond_get;
    pthread_cond_t cond_put;
    pthread_mutex_t lock;
};

// TODO: MAKE SYNCHRONIZED

/**
 * MQCreate: constructor for creating an MQ object.
 *
 * @param capacity: the message queue maximum capacity.
 *
 * @returns (MessageQueue) the newly created MessageQueue object
 * */
MessageQueue MQCreate(unsigned int capacity) {
    MessageQueue mq = (MessageQueue) malloc(sizeof(*mq));

    if (!mq && capacity != 0) {
        return NULL;
    }

    mq->size = 0;
    mq->capacity = capacity;
    mq->head = NULL;
    mq->tail = NULL;

    pthread_mutex_init(&mq->lock, NULL);
    pthread_cond_init(&mq->cond_get, NULL);
    pthread_cond_init(&mq->cond_put, NULL);
}

/**
 * MQFree: function for freeing the object and all of it's content from memory.
 *
 * @param messageQueue: the message queue object we free.
 * */
void MQFree(MessageQueue messageQueue) {
    Node iterator;
    Node next;
    Message currentMessage;

    if (messageQueue) {
        iterator = messageQueue->head;

        while (iterator != NULL) {
            currentMessage = iterator->message;

            next = iterator->next;

            MessageFree(currentMessage);
            free(iterator);

            iterator = next;
        }

        free(messageQueue);
    }
}

/**
 * MQPut: function for atomically putting a message in the queue.
 *
 * @param messageQueue: the message queue object we use put on.
 * @param message:      the message object to put in the queue.
 *
 * @returns (MQRetCode):
 *      MQ_ERR_NULL_ARGS - if messageQueue or message is Null
 *      MQ_ERR_MEMORY_FAIL - if a memory allocation failed.
 *      MQ_ERR_GENERAL_FAILURE - if operation failed for undocumented reason.
 *      MQ_SUCCESS - upon completion.
 * */
MQRetCode MQPut(MessageQueue messageQueue, Message message) {
    if (!messageQueue || !message) {
        return MQ_ERR_NULL_ARGS;
    }

    Message messageCopy = MessageCopy(message);

    if (!messageCopy) {
        return MQ_ERR_MEMORY_FAIL;
    }

    Node newNode = (Node) malloc(sizeof(*newNode));

    if (!newNode) {
        // rollback
        MessageFree(messageCopy);

        return MQ_ERR_MEMORY_FAIL;
    }

    newNode->next = NULL;
    newNode->message = messageCopy;

    mutex_lock(&messageQueue->lock);

    while (messageQueue->size == messageQueue->capacity) {
        cond_wait(&messageQueue->cond_get, &messageQueue->lock);
    }

    messageQueue->tail->next = newNode;
    ++messageQueue->size;
    messageQueue->tail = newNode;

    if (messageQueue->size == 1) {
        messageQueue->head = messageQueue->tail;
    }

    cond_signal(&messageQueue->cond_put);
    mutex_unlock(&messageQueue->lock);

    return MQ_SUCCESS;
}

/**
 * MQGet: function for atomically getting a message from the queue.
 *
 * @param messageQueue:     the message queue object we use get on.
 * @param message (out):    should be given an empty pointer of message struct to fill
 *                          with the first message of the queue.
 *
 * @returns (MQRetCode):
 *      MQ_ERR_NULL_ARGS - if messageQueue or message is Null
 *      MQ_ERR_GENERAL_FAILURE - if operation failed for undocumented reason.
 *      MQ_SUCCESS - upon completion.
 * */
MQRetCode MQGet(MessageQueue messageQueue, Message *message) {
    if (!messageQueue || !message) {
        return MQ_ERR_NULL_ARGS;
    }

    mutex_lock(&messageQueue->lock);

    while (messageQueue->size == 0) {
        cond_wait(&messageQueue->cond_put, &messageQueue->lock);
    }

    *message = messageQueue->head->message;
    --messageQueue->size;
    messageQueue->head = messageQueue->head->next;

    if (messageQueue->size == 0) {
        messageQueue->tail = messageQueue->head;
    }

    cond_signal(&messageQueue->cond_get);
    mutex_unlock(&messageQueue->lock);

    return MQ_SUCCESS;
}

/**
 * MQGetSize:
 *
 * @note:   unsafe function, can't verify if messageQueue is pointing to a real
 *          MessageQueue object.
 *
 * @param messageQueue: the target message queue object.
 *
 * @returns (unsigned int) the current size of the queue.
 * */
unsigned int MQGetSize(MessageQueue messageQueue) {
    return messageQueue->size;
}

/**
 * MQGetCapacity:
 *
 * @note:   unsafe function, can't verify if messageQueue is pointing to a real
 *          MessageQueue object.
 *
 * @param messageQueue: the target message queue object.
 *
 * @returns (unsigned int) the maximum capacity of the queue.
 * */
unsigned int MQGetCapacity(MessageQueue messageQueue) {
    return messageQueue->capacity;
}
