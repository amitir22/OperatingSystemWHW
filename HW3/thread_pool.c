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

    log("ThreadPoolCreate: start\n");

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

    tp->threads = (pthread_t *) malloc(capacity * sizeof(pthread_t));

    if (!tp->threads) {
        // rollback
        free(tp->functions);
        free(tp);

        return NULL;
    }

    tp->args = (void **) malloc(capacity * sizeof(void *));

    if (!tp->args) {
        // rollback
        free(tp->functions);
        free(tp->threads);
        free(tp);

        return NULL;
    }

    log("ThreadPoolCreate: done\n");

    return tp;
}

void ThreadPoolFree(ThreadPool threadPool) {
    log("ThreadPoolFree: start\n");

    if (threadPool) {
        if (threadPool->functions) {
            free(threadPool->functions);
        }

        if (threadPool->threads) {
            free(threadPool->threads);
        }

        free(threadPool);
    }

    log("ThreadPoolFree: done\n");
}

ThreadIndex TPAddThread(ThreadPool threadPool, ThreadFunction function, void *args) {
    ThreadIndex threadIndex;

    log("TPAddThread: start\n");

    if (!threadPool) {
        return -1;
    }

    threadIndex = threadPool->size++;

    threadPool->functions[threadIndex] = function;
    threadPool->args[threadIndex] = args;

    log("TPAddThread: done\n");

    return threadIndex;
}

void TPSignalStartAll(ThreadPool threadPool) {
    pthread_t currentTID;

    log("TPSignalStartAll: start\n");

    for (int i = 0; i < threadPool->size; ++i) {
        pthread_create(&currentTID, NULL, threadPool->functions[i], threadPool->args[i]);
        threadPool->threads[i] = currentTID;

        // todo: remove:
        if (IS_DEBUG) {
            printf("TPSignalStartAll: thread id: %ld created\n", currentTID);
        }
    }

    log("TPSignalStartAll: done\n");
}

unsigned int TPGetPoolSize(ThreadPool threadPool) {
    if (!threadPool) {
        return -1; // todo: maybe define macro: INVALID_VALUE
    }

    return threadPool->capacity;
}