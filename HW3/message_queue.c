#include "message_queue.h"

typedef struct t_node {
    struct t_node *next;
    Message message;
} *Node;

struct t_message_queue {
    int capacity;
    int size;
    int randomDropCount;
    Node head;
    Node tail;
    MQSchedPolicy schedPolicy;

    pthread_mutex_t lock;
    pthread_cond_t cond_get;
    pthread_cond_t cond_put;
};

// helper functions
int* getRandomIndexes(int capacity, int amount);
MQSchedPolicy stringToPolicy(char *schedAlgo);
void putHandleBlock(MessageQueue messageQueue);
void putHandleDropTail(Content **dropped, int *droppedAmount, Node *newNode, int *isPutting,
                       MQRetCode *result);
void putHandleDropHead(MessageQueue messageQueue, Content **dropped, int *droppedAmount,
                       MQRetCode *result);
void putHandleDropRandom(MessageQueue messageQueue, Content **dropped, int *droppedAmount,
                         MQRetCode *result);

// TODO: remove commented code

/**
 * MQCreate: constructor for creating an MQ object.
 *
 * @param capacity:     the message queue maximum capacity.
 * @param schedAlgo:    the scheduling algorithm chosen
 *
 * @returns (MessageQueue) the newly created MessageQueue object. (NULL if fails)
 * */
MessageQueue MQCreate(int capacity, char *schedAlgo) {
    MessageQueue mq = (MessageQueue) malloc(sizeof(*mq));

    log("MQCreate: start\n");

    if (!mq && capacity != 0) {
        return NULL;
    }

    mq->size = 0;
    mq->capacity = capacity;
    mq->head = NULL;
    mq->tail = NULL;
    mq->randomDropCount = capacity / 4;
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

            free(currentMessage->metaData);
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
 * @param messageQueue:         the message queue object we use put on.
 * @param message:              the message object to put in the queue.
 * @param dropped:              the array of contents to drop as output for the caller.
 *                              (expecting an uninitialized array)
 * @param droppedAmount (out):  the amount of contents dropped as output for the caller.
 *
 * @note:   the params with '(out)' means that those parameters should not be initialized before
 *          since they'll be overwritten.
 *
 * @returns (MQRetCode):
 *      MQ_SUCCESS          - upon completion.
 *      MQ_DROP             - when a dropping policy is activated
 *      MQ_ERR_NULL_ARGS    - if messageQueue or message are Null.
 *      MQ_ERR_MEMORY_FAIL  - if a memory allocation failed.
 * */
MQRetCode MQPut(MessageQueue messageQueue, Message message, Content **dropped, int *droppedAmount) {
    Message messageCopy;
    Node newNode;
    int isPutting = 1;
    MQRetCode result = MQ_SUCCESS;

    log("MQPut: start\n");

    if (!messageQueue || !message) {
        return MQ_ERR_NULL_ARGS;
    }

    messageCopy = MessageCopy(message);

    if (!messageCopy) {
        log("MQPut: failed: memory allocation fail: MessageCopy\n");
        return MQ_ERR_MEMORY_FAIL;
    }

    newNode = (Node) malloc(sizeof(*newNode));

    if (!newNode) {
        // rollback
        free(messageCopy->metaData);
        MessageFree(messageCopy);

        log("MQPut: failed: memory allocation fail: newNode\n");

        return MQ_ERR_MEMORY_FAIL;
    }

    log("MQPut: memory allocation successful\n");

    newNode->next = NULL;
    newNode->message = messageCopy;

    // Enter critical section
    pthread_mutex_lock(&messageQueue->lock);

    while (messageQueue->size == messageQueue->capacity) {
        log("MQPut: size == capacity\n");

        if (messageQueue->schedPolicy == BLOCK) {
            putHandleBlock(messageQueue);
        } else if (messageQueue->schedPolicy == DROP_TAIL) {
            putHandleDropTail(dropped, droppedAmount, &newNode, &isPutting, &result);

            break;
        } else if (messageQueue->schedPolicy == DROP_HEAD) {
            putHandleDropHead(messageQueue, dropped, droppedAmount, &result);
        } else if (messageQueue->schedPolicy == DROP_RANDOM) {
            putHandleDropRandom(messageQueue, dropped, droppedAmount, &result);

            if (result == MQ_ERR_MEMORY_FAIL) {
                return result;
            }

            break;
        }
    }

    // if needs to update the MQ inner data members
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
 *      MQ_ERR_NULL_ARGS    - if messageQueue or message are Null
 *      MQ_SUCCESS          - upon completion.
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
 * MQGetSize: MQ size getter (size is the number of elements in the queue)
 *
 * @param messageQueue: the target message queue object.
 *
 * @returns (int) the current size of the queue. (returning HW_INVALID_VALUE(-1) if null argument)
 * */
int MQGetSize(MessageQueue messageQueue) {
    if (!messageQueue) {
        return HW3_INVALID_VALUE;
    }

    return messageQueue->size;
}

/**
 * MQGetCapacity: MQ capacity getter
 *
 * @param messageQueue: the target message queue object.
 *
 * @returns (int) the maximum capacity of the queue. (returning HW_INVALID_VALUE(-1) if null argument)
 * */
int MQGetCapacity(MessageQueue messageQueue) {
    if (!messageQueue) {
        return HW3_INVALID_VALUE;
    }

    return messageQueue->capacity;
}

// Helper functions implementation:

int* getRandomIndexes(int capacity, int amount) {
    int *randIndexes;
    int currentIndex;

    randIndexes = (int *) malloc(capacity * sizeof(int));

    if (!randIndexes) {
        return NULL;
    }

    // init randIndexes
    for (int i = 0; i < capacity; ++i) {
        randIndexes[i] = 0;
    }

    // fill randIndexes
    for (int i = 0; i < amount; ++i) {
        currentIndex = rand() % capacity;

        if (randIndexes[currentIndex] == 0) {
            randIndexes[currentIndex] = 1;
        } else {
            --i;
        }
    }

    return randIndexes;
}

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

void putHandleBlock(MessageQueue messageQueue) {
    log("MQPut: putHandleBlock\n");

    pthread_cond_wait(&messageQueue->cond_get, &messageQueue->lock);
}

void putHandleDropTail(Content **dropped, int *droppedAmount, Node *newNode, int *isPutting,
                       MQRetCode *result) {
    log("MQPut: putHandleDropTail\n");

    (*dropped)[0] = (*newNode)->message->content;
    *isPutting = 0;
    *result = MQ_DROP;
    *droppedAmount = 1;

    // freeing the new node that was supposed to be inserted but then dropped
    free((*newNode)->message->metaData);
    MessageFree((*newNode)->message);
    free((*newNode));
}

void putHandleDropHead(MessageQueue messageQueue, Content **dropped, int *droppedAmount,
                       MQRetCode *result) {
    Node nodeToFree;

    log("MQPut: putHandleDropHead\n");

    nodeToFree = messageQueue->head;
    messageQueue->head = messageQueue->head->next;
    (*dropped)[0] = nodeToFree->message->content;

    free(nodeToFree->message->metaData);
    MessageFree(nodeToFree->message);
    free(nodeToFree);

    --messageQueue->size;

    if (messageQueue->size == 0) {
        messageQueue->tail = messageQueue->head;
    }

    *droppedAmount = 1;
    *result = MQ_DROP;
}

void putHandleDropRandom(MessageQueue messageQueue, Content **dropped, int *droppedAmount,
                         MQRetCode *result) {
    Node iterator;
    Node prev;
    Node nodeToFree;
    int *randomIndexes;
    int droppedIndex;

    log("MQPut: putHandleDropRandom\n");

    if (messageQueue->capacity < 4) {
        *droppedAmount = 0;
        putHandleBlock(messageQueue);
    } else {
        *droppedAmount = messageQueue->randomDropCount;

        randomIndexes = getRandomIndexes(messageQueue->capacity, *droppedAmount);

        if (!randomIndexes) {
            *result = MQ_ERR_MEMORY_FAIL;
            return;
        }

        iterator = messageQueue->head;
        droppedIndex = 0;

        // removing all the random selected messages from the first indexes
        while (randomIndexes[droppedIndex]) {
            (*dropped)[droppedIndex] = messageQueue->head->message->content;
            nodeToFree = messageQueue->head;
            messageQueue->head = messageQueue->head->next;

            --messageQueue->size;
            ++droppedIndex;
            iterator = iterator->next;

            free(nodeToFree->message->metaData);
            MessageFree(nodeToFree->message);
            free(nodeToFree);
        }

        prev = iterator;
        iterator = iterator->next;

        // removing the rest of the random selected messages
        for (int i = droppedIndex + 1; i < messageQueue->capacity - 1; ++i) {
            if (randomIndexes[i]) {
                (*dropped)[droppedIndex] = iterator->message->content;
                nodeToFree = iterator;
                prev->next = iterator->next;
                iterator = iterator->next;
                free(nodeToFree->message->metaData);
                MessageFree(nodeToFree->message);
                free(nodeToFree);

                --messageQueue->size;
                ++droppedIndex;
            } else {
                prev = iterator;
                iterator = iterator->next;
            }
        }

        if (prev->next == NULL) {
            messageQueue->tail = prev;
        }


        free(randomIndexes);
        *result = MQ_DROP;
    }
}
