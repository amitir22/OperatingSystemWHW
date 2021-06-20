#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>
#include "logger.h"

#define HW3_INVALID_VALUE (-1)

typedef int Content;

typedef struct t_message_meta_data {
    struct timeval arrivalTime;
    struct timeval dispatchTime;
    int threadID;
    int requestsCount;
    int numStaticRequests;
    int numDynamicRequests;
} *MessageMetaData;

typedef struct t_message {
    Content content;
    MessageMetaData metaData;
} *Message;

Message MessageCreate(Content content, MessageMetaData metaData);
void MessageFree(Message message);
Message MessageCopy(Message other);
MessageMetaData buildMessageMetaData();

#endif //MESSAGE_H
