#ifndef MESSAGE_H
#define MESSAGE_H
#incldue "segel.h"

typedef struct t_message *Message;

Message MessageCreate(char *content);
Message MessageCopy(Message other);
void MessageFree(Message message);

#endif //MESSAGE_H
