#include "thread_pool.h"

struct t_thread_pool {
    unsigned int capacity;
    unsigned int size;

    ThreadFunction *functions;
    void **args;
    pthread_t *threads;
};

ThreadPool ThreadPoolCreate(unsigned int capacity) {
    ThreadPool tp = (ThreadPool) malloc(sizeof(*tp));

    if (!tp) {
        return NULL;
    }

    tp->capacity = capacity;
    tp->size = 0;
    tp->functions = (ThreadFunction *) malloc(capacity * sizeof(ThreadFunction));

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

    tp->args = (pthread_t *) malloc(capacity * sizeof(void *);

    if (!tp->args) {
        // rollback
        free(tp->functions);
        free(tp->threads);
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

ThreadIndex TPAddThread(ThreadPool threadPool, ThreadFunction function, void *args) {
    ThreadIndex threadIndex;

    if (!threadPool) {
        return -1;
    }

    threadIndex = threadPool->size;

    threadPool->functions[threadIndex] = function;
    threadPool->args[threadIndex] = args;
}

void TPSignalStartAll(ThreadPool threadPool) {
    pthread_t currentTID;

    for (int i = 0; i < threadPool->size; ++i) {
        pthread_create(&currentTID, NULL, threadPool->functions[i], threadPool->args[i]);
        threadPool->threads[i] = currentTID;
    }
}
