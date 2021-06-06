#include "message.h"

struct t_message {
    char *content;
};

/**
 * MessageCreate: constructs a Message object.
 *
 * @param content: the content of the message.
 *
 * @returns: a Message object (or NULL if fails).
 * */
Message MessageCreate(char *content) {
    Message message = (Message) malloc(sizeof(*message));

    if (!message) {
        // allocation failed
        return NULL;
    }

    message->content = (char *) malloc(strlen(content) + 1);

    if (!message->content) {
        // allocation failed, rollback
        free(message);
        return NULL;
    }

    strcpy(message->content, content);

    return message;
}

/**
 * MessageCopy: copies a given Message object.
 *
 * @param other: the message the copy.
 *
 * @returns: a copy of the given `other` message.
 * */
Message MessageCopy(Message other) {
    if (!other) {
        return NULL;
    }

    Message copy = MessageCreate(other->content);

    return copy;
}

/**
 * MessageFree: frees the memory of the Message object.
 *
 * @param message: the message to free.
 * */
void MessageFree(Message message) {
    if (message) {
        if (message->content) {
            free(message->content);
        }

        free(message);
    }
}
