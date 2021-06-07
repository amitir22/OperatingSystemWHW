#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdlib.h>
#include <pthread.h>

typedef struct t_thread_pool *ThreadPool;

typedef void (*ThreadFunction)(MessageQueue);

typedef enum e_mq_return_code {
    TP_SUCCESS,
    TP_FAIL
} TPRetCode;

typedef int ThreadIndex;

ThreadPool ThreadPoolCreate(unsigned int capacity)
void ThreadPoolFree(ThreadPool threadPool);
ThreadIndex TPAddThread(ThreadPool threadPool, ThreadFunction function, void *args);
void TPSignalStartAll(ThreadPool threadPool);

#endif //THREAD_POOL_H
