#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>

typedef union u_content {
    int fd;
    char *str;
} Content;

typedef enum e_message_content_type {
    MSG_INT,
    MSG_STR
} MessageContentType;

typedef struct t_message {
    Content content;
    MessageContentType contentType;
} *Message;

Message MessageCreate(Content content, MessageContentType type);
void MessageFree(Message message);
Message MessageCopy(Message other);

#endif //MESSAGE_H
