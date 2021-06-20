#include "thread_pool.h"

struct t_thread_pool {
    unsigned int capacity;
    unsigned int size;

    ThreadFunction *functions;
    void **args;
    pthread_t *threads;
};

/**
 * ThreadPoolCreate: creating a thread pool object.
 *
 * @param: capacity: the number of threads in the thread pool.
 *
 * @returns: the created ThreadPool. (NULL if fails)
 * */
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

/**
 * ThreadPoolFree: freeing a thread pool object.
 *
 * @param: threadPool: the thread pool to free.
 * */
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

/**
 * TPAddThread: adding a thread to the thread pool.
 *
 * @param: threadPool:  the thread pool object to add to.
 * @param: function:    the function of the added thread to run when signaled.
 * @param: args:        the args for the function of the added thread.
 *
 * @returns: the index of the newly added thread. (HW3_INVALID_VALUE(-1) if fails)
 * */
ThreadIndex TPAddThread(ThreadPool threadPool, ThreadFunction function, void *args) {
    ThreadIndex threadIndex;

    log("TPAddThread: start\n");

    if (!threadPool) {
        return HW3_INVALID_VALUE;
    }

    threadIndex = threadPool->size++;

    threadPool->functions[threadIndex] = function;
    threadPool->args[threadIndex] = args;

    log("TPAddThread: done\n");

    return threadIndex;
}

/**
 * TPSignalStartAll: signaling all the threads in the thread pool to start running.
 *
 * @param: threadPool: the thread pool of the threads we signal to start.
 * */
void TPSignalStartAll(ThreadPool threadPool) {
    pthread_t currentTID;

    log("TPSignalStartAll: start\n");

    for (int i = 0; i < threadPool->size; ++i) {
        pthread_create(&currentTID, NULL, threadPool->functions[i], threadPool->args[i]);
        threadPool->threads[i] = currentTID;

        if (IS_DEBUG) {
            printf("TPSignalStartAll: thread id: %ld created\n", currentTID);
        }
    }

    log("TPSignalStartAll: done\n");
}

/**
 * TPGetPoolSize: getting the pool size of the queried thread pool.
 *
 * @param: threadPool: the thread pool object we query.
 *
 * @returns: the size of the thread pool. (assuming the given thread pool is valid)
 * */
unsigned int TPGetPoolSize(ThreadPool threadPool) {
    return threadPool->capacity;
}