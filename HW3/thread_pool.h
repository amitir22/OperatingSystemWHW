#ifndef THREAD_POOL_H
#define THREAD_POOL_H

typedef struct t_thread_pool *ThreadPool;

typedef enum e_mq_return_code {
    TP_SUCCESS,
    TP_FAIL
} TPRetCode;

ThreadPool ThreadPoolCreate();
void ThreadPoolFree(ThreadPool threadPool);

#endif //THREAD_POOL_H
