#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>
#include "logger.h"

typedef union u_content {
    int fd;
    char *str;
} Content;

typedef enum e_message_content_type {
    MSG_INT,
    MSG_STR
} MessageContentType;

typedef struct t_message_meta_data {
    int arrivalTime;
    int dispatchTime;
    int threadID;
    int threadCount;
    int numStaticRequests;
    int numDynamicRequests;
} MessageMetaData;

typedef struct t_message {
    Content content;
    MessageContentType contentType;
    // todo: add metadata variables
} *Message;

Message MessageCreate(Content content, MessageContentType type);
void MessageFree(Message message);
Message MessageCopy(Message other);

#endif //MESSAGE_H
