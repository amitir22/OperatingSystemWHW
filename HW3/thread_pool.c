#include "thread_pool.h"

struct t_thread_pool {
    unsigned int capacity;
    unsigned int size;

    ThreadFunction *functions;
    pthread_t *threads;
};

ThreadPool ThreadPoolCreate(unsigned int capacity) {
    ThreadPool tp = (ThreadPool) malloc(sizeof(*tp));

    if (!tp) {
        return NULL;
    }

    tp->capacity = capacity;
    tp->size = 0;
    tp->functions = (ThreadFunction *) malloc(capacity * sizeof(*(tp->functions))); // todo: make sure

    if (!tp->functions) {
        // rollback
        free(tp);

        return NULL;
    }

    tp->threads = (pthread_t *) malloc(capacity * sizeof(pthread_t);

    if (!tp->threads) {
        // rollback
        free(tp->functions);
        free(tp);

        return NULL;
    }

    return tp;
}

void ThreadPoolFree(ThreadPool threadPool) {
    if (threadPool) {
        if (threadPool->functions) {
            free(threadPool->functions);
        }

        if (threadPool->threads) {
            free(threadPool->threads);
        }

        free(threadPool);
    }
}

ThreadIndex TPAddThread(ThreadPool threadPool, ThreadFunction function) {
    ThreadIndex threadIndex = threadPool->size;

    if (!threadPool) {
        return -1;
    }

    threadPool->functions[threadIndex] = function;
}

void TPSignalStartAll(ThreadPool threadPool) {
    for (int i = 0; i < threadPool->size; ++i) {
        pthread_create(threadPool->threads + i, NULL, threadPool->functions[i], NULL);
    }
}
