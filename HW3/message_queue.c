#include "message_queue.h"

typedef struct t_node {
    struct t_node *next;
    Message message;
} *Node;

struct t_message_queue {
    int capacity;
    int size;
    Node head;
    Node tail;
    MessageContentType messageContentType;
    MQSchedPolicy schedPolicy;

    pthread_mutex_t lock;
    pthread_cond_t cond_get;
    pthread_cond_t cond_put;
};

// helper function
MQSchedPolicy stringToPolicy(char *schedAlgo) {
    const char *blockPolicy = "block";
    const char *dropTailPolicy = "dt";
    const char *dropHeadPolicy = "dh";
    const char *randomPolicy = "random";

    if (0 == strcmp(schedAlgo, blockPolicy)) {
        return BLOCK;
    } else if (0 == strcmp(schedAlgo, dropTailPolicy)) {
        return DROP_TAIL;
    } else if (0 == strcmp(schedAlgo, dropHeadPolicy)) {
        return DROP_HEAD;
    } else if (0 == strcmp(schedAlgo, randomPolicy)) {
        return DROP_RANDOM;
    } else {
        log("message_queue.c: stringToPolicy: unknown policy\n");
        return INVALID;
    }
}

int calcNumToRandomDrop(MessageQueue messageQueue) {
    if (!messageQueue) {
        return HW3_INVALID_VALUE;
    }

    return (int) (0.25 * messageQueue->capacity);
}

/**
 * MQCreate: constructor for creating an MQ object.
 *
 * @param capacity: the message queue maximum capacity.
 *
 * @returns (MessageQueue) the newly created MessageQueue object.
 * */
MessageQueue MQCreate(int capacity, MessageContentType messageType, char *schedAlgo) {
    MessageQueue mq = (MessageQueue) malloc(sizeof(*mq));

    log("MQCreate: start\n");

    if (!mq && capacity != 0) {
        return NULL;
    }

    mq->size = 0;
    mq->capacity = capacity;
    mq->head = NULL;
    mq->tail = NULL;
    mq->messageContentType = messageType;
    mq->schedPolicy = stringToPolicy(schedAlgo);

    if (mq->schedPolicy == INVALID) {
        //  rollback
        free(mq);
        mq = NULL;

        return NULL;
    }

    pthread_mutex_init(&mq->lock, NULL);
    pthread_cond_init(&mq->cond_get, NULL);
    pthread_cond_init(&mq->cond_put, NULL);

    log("MQCreate: done\n");

    return mq;
}

/**
 * MQFree: function for freeing the object and all of it's content from memory.
 *
 * @param messageQueue: the message queue object we free.
 * */
void MQFree(MessageQueue messageQueue) {
    log("MQFree: start\n");

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

        pthread_mutex_destroy(&messageQueue->lock);
        pthread_cond_destroy(&messageQueue->cond_get);
        pthread_cond_destroy(&messageQueue->cond_put);

        free(messageQueue);
    }

    log("MQFree: done\n");
}

/**
 * MQPut: function for atomically putting a message in the queue.
 *
 * @param messageQueue: the message queue object we use put on.
 * @param message:      the message object to put in the queue.
 *
 * @returns (MQRetCode):
 *      MQ_SUCCESS - upon completion.
 *      MQ_DROP - when dropping policy is activated
 *      MQ_ERR_NULL_ARGS - if messageQueue or message is Null.
 *      MQ_ERR_MEMORY_FAIL - if a memory allocation failed.
 *      MQ_ERR_MISMATCH_CONTENT_TYPE - if the message content type doesn't match.
 *      MQ_ERR_GENERAL_FAILURE - if operation failed for undocumented reason.
 * */
MQRetCode MQPut(MessageQueue messageQueue, Message message, Content *dropped) {
    MQRetCode result = MQ_SUCCESS;
    Message messageCopy;
    Node newNode;
    int isPutting = 1;

    // used for DROP HEAD
    Node nodeToFree;

    log("MQPut: start\n");

    if (!messageQueue || !message) {
        return MQ_ERR_NULL_ARGS;
    }

    if (messageQueue->messageContentType != message->contentType) {
        return MQ_ERR_MISMATCH_CONTENT_TYPE;
    }

    messageCopy = MessageCopy(message);

    if (!messageCopy) {
        log("MQPut: failed: memory fail 1\n");

        return MQ_ERR_MEMORY_FAIL;
    }

    newNode = (Node) malloc(sizeof(*newNode));

    if (!newNode) {
        // rollback
        MessageFree(messageCopy);

        log("MQPut: failed: memory fail 2\n");

        return MQ_ERR_MEMORY_FAIL;
    }

    log("MQPut: memory allocation successful\n");

    newNode->next = NULL;
    newNode->message = messageCopy;

    // Enter critical section
    pthread_mutex_lock(&messageQueue->lock);

    while (messageQueue->size == messageQueue->capacity) {
        if (messageQueue->schedPolicy == BLOCK) {
            log("MQPut: BLOCK\n");

            pthread_cond_wait(&messageQueue->cond_get, &messageQueue->lock);
        } else if (messageQueue->schedPolicy == DROP_TAIL) {
            log("MQPut: DROP_TAIL\n");

            // rollback of allocations (dropping the new message)
            *dropped = newNode->message->content;
            MessageFree(newNode->message);
            free(newNode);

            isPutting = 0;
            result = MQ_DROP;
            break;
        } else if (messageQueue->schedPolicy == DROP_HEAD) {
            log("MQPut: DROP_HEAD\n");

            nodeToFree = messageQueue->head;
            messageQueue->head = messageQueue->head->next;

            *dropped = nodeToFree->message->content;
            MessageFree(nodeToFree->message);
            free(nodeToFree);

            --messageQueue->size; // update size

            if (messageQueue->size == 0) {
                messageQueue->tail = messageQueue->head; // update tail
            }

            result = MQ_DROP;
        } else if (messageQueue->schedPolicy == DROP_RANDOM) {
            log("MQPut: DROP_RANDOM\n");

            // TODO:
        }
    }

    if (isPutting) {
        ++messageQueue->size;

        if (messageQueue->size == 1) {
            messageQueue->tail = newNode;
            messageQueue->head = newNode;
        } else {
            messageQueue->tail->next = newNode;
            messageQueue->tail = newNode;
        }
    }

    pthread_cond_signal(&messageQueue->cond_put);
    pthread_mutex_unlock(&messageQueue->lock);
    // Escape critical section

    log("MQPut: done\n");

    if (IS_DEBUG) {
        printf("MQPut: result=%d\n", result);
    }

    return result;
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
    Node toFree;

    log("MQGet: start\n");

    if (!messageQueue || !message) {
        return MQ_ERR_NULL_ARGS;
    }

    // Enter critical section
    pthread_mutex_lock(&messageQueue->lock);

    while (messageQueue->size == 0) {
        pthread_cond_wait(&messageQueue->cond_put, &messageQueue->lock);
    }

    toFree = messageQueue->head;
    messageQueue->head = messageQueue->head->next; // update head

    *message = MessageCopy(toFree->message); // get the message itself from the node
    MessageFree(toFree->message);
    free(toFree); // free the node memory

    --messageQueue->size; // update size

    if (messageQueue->size == 0) {
        messageQueue->tail = messageQueue->head; // update tail
    }

    pthread_cond_signal(&messageQueue->cond_get);
    pthread_mutex_unlock(&messageQueue->lock);
    // Escape critical section

    log("MQGet: done\n");

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
 * @returns (int) the current size of the queue.
 * */
int MQGetSize(MessageQueue messageQueue) {
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
 * @returns (int) the maximum capacity of the queue.
 * */
int MQGetCapacity(MessageQueue messageQueue) {
    return messageQueue->capacity;
}
