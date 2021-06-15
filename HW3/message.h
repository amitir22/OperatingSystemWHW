#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>
#include "logger.h"

#define HW3_INVALID_VALUE (-1)

typedef union u_content {
    int fd;
    char *str;
} Content;

typedef enum e_message_content_type {
    MSG_INT,
    MSG_STR
} MessageContentType;

typedef struct t_message_meta_data {
    unsigned long arrivalTimeMS;
    unsigned long dispatchTimeMS;
    int threadID;
    int requestsCount;
    int numStaticRequests;
    int numDynamicRequests;
} *MessageMetaData;

typedef struct t_message {
    Content content;
    MessageContentType contentType;
    MessageMetaData metaData;
} *Message;

Message MessageCreate(Content content, MessageContentType type, MessageMetaData metaData);
void MessageFree(Message message);
Message MessageCopy(Message other);
MessageMetaData buildMessageMetaData();

#endif //MESSAGE_H
