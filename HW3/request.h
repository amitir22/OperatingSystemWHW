#ifndef __REQUEST_H__

#include "message.h"

long unsigned int convertTimeValToMSULong(struct timeval timeV);
int requestHandle(int fd, MessageMetaData metaData);

#endif
