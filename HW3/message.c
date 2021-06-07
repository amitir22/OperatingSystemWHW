#include "message.h"

/**
 * MessageCreate: constructs a Message object.
 *
 * @param content: the content of the message.
 *
 * @returns: a Message object (or NULL if fails).
 * */
Message MessageCreate(Content content, MessageContentType contentType) {
    Message message = (Message) malloc(sizeof(*message));

    if (!message) {
        // allocation failed
        return NULL;
    }

    message->contentType = contentType;

    if (contentType == MSG_INT) {
        message->content.fd = content.fd;
    } else if (contentType == MSG_STR) {
        message->content.str = (char *) malloc(strlen(content.str) + 1);

        if (message->content.str == NULL) {
            // allocation failed, rollback
            free(message);
            return NULL;
        }

        strcpy(message->content.str, content.str);
    } else { // unknown content type
        // rollback
        free(message);
    }

    return message;
}

/**
 * MessageFree: frees the memory of the Message object.
 *
 * @param message: the message to free.
 * */
void MessageFree(Message message) {
    if (message) {
        if (message->contentType == MSG_STR && message->content.str) {
            free(message->content.str);
        }

        free(message);
    }
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

    Message copy = MessageCreate(other->content, other->contentType);

    return copy;
}
