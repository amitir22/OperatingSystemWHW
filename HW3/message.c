#include "message.h"

/**
 * MessageCreate: constructs a Message object.
 *
 * @param content: the content of the message.
 *
 * @returns: a Message object (or NULL if fails).
 * */
Message MessageCreate(Content content, MessageContentType contentType, MessageMetaData metaData) {
    Message message = (Message) malloc(sizeof(*message));

    log("MessageCreate: start\n");

    if (!message) {
        // allocation failed
        return NULL;
    }

    message->contentType = contentType;
    message->metaData = metaData;

    if (contentType == MSG_INT) {
        message->content.fd = content.fd;
    } else if (contentType == MSG_STR) {
        message->content.str = (char *) malloc(strlen(content.str) + 1);

        if (message->content.str == NULL) {
            // rollback
            free(message);
            message = NULL;
        } else {
            strcpy(message->content.str, content.str);
        }
    } else { // unknown content type
        // rollback
        free(message);
        message = NULL;
    }

    if (!message) {
        log("MessageCreate: failed\n");

        return NULL;
    } else {
        log("MessageCreate: done\n");
    }

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
        log("MessageCopy: failed because of NULL argument\n");

        return NULL;
    }

    copy = MessageCreate(other->content, other->contentType, other->metaData);

    log("MessageCopy: done\n");

    return copy;
}

MessageMetaData buildMessageMetaData() {
    MessageMetaData metaData;

    log("buildMessageMetaData: start\n");

    metaData = (MessageMetaData) malloc(sizeof(*metaData));

    if (!metaData) {
        log("buildMessageMetaData: failed with memory allocation\n");

        return NULL;
    }

    log("buildMessageMetaData: done\n");

    return metaData;
}
