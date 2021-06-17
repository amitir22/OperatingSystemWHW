#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// todo: make sure it's 0 before submission
#define IS_DEBUG 1
#define log(MESSAGE) if (IS_DEBUG) printf((MESSAGE))

#endif //LOGGER_H
