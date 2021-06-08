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

    log("MessageCreate: start\n");

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
            // rollback
            free(message);
            return NULL;
        }

        strcpy(message->content.str, content.str);
    } else { // unknown content type
        // rollback
        free(message);
        return NULL;
    }

    if (!message) {
        log("MessageCreate: failed\n");

        return NULL;
    }

    log("MessageCreate: done\n");

    return message;
}

/**
 * MessageFree: frees the memory of the Message object.
 *
 * @param message: the message to free.
 * */
void MessageFree(Message message) {
    log("MessageFree: start\n");

    if (message) {
        if (message->contentType == MSG_STR && message->content.str) {
            free(message->content.str);
        }

        free(message);
    }

    log("MessageFree: done\n");
}

/**
 * MessageCopy: copies a given Message object.
 *
 * @param other: the message the copy.
 *
 * @returns: a copy of the given `other` message.
 * */
Message MessageCopy(Message other) {
    Message copy;

    log("MessageCopy: start\n");

    if (!other) {
        return NULL;
    }

    copy = MessageCreate(other->content, other->contentType);

    log("MessageCopy: done\n");

    return copy;
}
