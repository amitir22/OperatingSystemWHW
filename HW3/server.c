#include "segel.h"
#include "request.h"
#include "message_queue.h"
#include "thread_pool.h"
#include "logger.h"

#define SCHED_ALG_MAX_SIZE 7

typedef struct t_worker_thread_params {
    MessageQueue connectionsQueue;
    ThreadIndex threadID;
} *WorkerThreadParams;

// HW3: Parse the new arguments too
void getargs(int *port, int *threadPoolSize, int *queueSize, char **schedAlg,
             int argc, char *argv[]) {
    if (argc < 2) {
	    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	    exit(1);
    }

    // TODO: maybe validate parameters?
    *port = atoi(argv[1]);
    *threadPoolSize = atoi(argv[2]);
    *queueSize = atoi(argv[3]);

    strcpy(*schedAlg, argv[4]);
}

struct timeval getCurrentTime() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return currentTime;
}

void* workerThreadJob(void *params) {
    struct timeval currentTime;
    WorkerThreadParams currentThreadParams = (WorkerThreadParams) params;
    MessageQueue connectionsQueue = currentThreadParams->connectionsQueue;
    ThreadIndex currentThreadID = currentThreadParams->threadID;
    Message currentConnectionMessage;
    MessageMetaData currentMessageMetaData;
    MQRetCode getRetCode;
    int currentConnFd;
    int currentThreadJobCount = 0;
    int currentThreadDynamicCount = 0;
    int currentThreadStaticCount = 0;
    int currentIsStatic;
    long unsigned int currentArrivalTimeMS;
    long unsigned int currentDispatchTimeMS;

    log("workerThreadJob: start\n");

    while (1) {
        getRetCode = MQGet(connectionsQueue, &currentConnectionMessage);

        currentTime = getCurrentTime();
        currentMessageMetaData = currentConnectionMessage->metaData;
        currentMessageMetaData->threadID = currentThreadID;
        timersub(&currentTime, &(currentMessageMetaData->arrivalTime), &(currentMessageMetaData->dispatchTime));

        currentArrivalTimeMS = convertTimeValToMSULong(currentMessageMetaData->arrivalTime);
        currentDispatchTimeMS = convertTimeValToMSULong(currentMessageMetaData->dispatchTime);

        if (getRetCode == MQ_SUCCESS) {
            currentConnFd = currentConnectionMessage->content.fd;
            ++currentThreadJobCount;
            currentMessageMetaData->requestsCount = currentThreadJobCount;
            currentMessageMetaData->numStaticRequests = currentThreadStaticCount;
            currentMessageMetaData->numDynamicRequests = currentThreadDynamicCount;

            if (IS_DEBUG) {
                printf("workerThreadJob: %d, Metadata:\n", currentMessageMetaData->threadID);
                printf("\trequest count: %d\n", currentMessageMetaData->requestsCount);
                printf("\tstatic count: %d\n", currentMessageMetaData->numStaticRequests);
                printf("\tdynamic count: %d\n", currentMessageMetaData->numDynamicRequests);
                printf("\tarrival time: %lu\n", currentArrivalTimeMS);
                printf("\tdispatch time: %lu\n", currentDispatchTimeMS);
            }

            currentIsStatic = requestHandle(currentConnFd, currentMessageMetaData);

            // updating thread counters
            if (currentIsStatic == HW3_INVALID_VALUE) {
                log("server.c: workerThreadJob: invalid request arrived\n");
            } else if (currentIsStatic) {
                ++currentThreadStaticCount;
            } else {
                ++currentThreadDynamicCount;
            }

            free(currentMessageMetaData);
            MessageFree(currentConnectionMessage);

            Close(currentConnFd);

            log("server.c: workerThreadJob: finished requestHandle\n");
        } else {
            log("workerThreadJob: error with MQGet \n");
            break;
        }

        log("workerThreadJob: iteration done\n");
    }

    return NULL;
}

int initTP(ThreadPool *threadPool, int threadPoolSize) {
    *threadPool = ThreadPoolCreate(threadPoolSize);

    if (!(*threadPool)) {
        return 0;
    }

    return 1;
}

int initMQ(MessageQueue *connectionsQueue, int queueSize, char *schedAlgo) {
    *connectionsQueue = MQCreate(queueSize, MSG_INT, schedAlgo);

    if (!(*connectionsQueue)) {
        return 0;
    }

    return 1;
}

int initServerDataStructures(ThreadPool *threadPool, int threadPoolSize, char *schedAlgo,
                             MessageQueue *connectionsQueue, int queueSize) {
    if (initTP(threadPool, threadPoolSize)) {
        log("initServerDataStructures: initTP successful\n");

        if (initMQ(connectionsQueue, queueSize, schedAlgo)) {
            log("initServerDataStructures: initMQ successful\n");

            return 1;
        } else {
            // rollback
            log("initServerDataStructures: initMQ failed, rolling back\n");;
            ThreadPoolFree(*threadPool);
            log("initServerDataStructures: rollback successful, threadPool has been freed\n");
        }
    }

    return 0;
}

void initWorkerThreads(ThreadPool threadPool, MessageQueue connectionsQueue) {
    WorkerThreadParams params;

    log("initWorkerThreads: start\n");

    for (int i = 0; i < TPGetPoolSize(threadPool); i++) {
        params = (WorkerThreadParams) malloc(sizeof(*params));

        if (!params) {
            // todo: handle fail?
            log("initWorkerThreads: failed\n");

            return;
        }

        params->connectionsQueue = connectionsQueue;

        params->threadID = i;
        TPAddThread(threadPool, workerThreadJob, params);
    }

    log("initWorkerThreads: end\n");
}

int startServer(int port, int threadPoolSize, int queueSize, char *schedAlgo) {
    int listenfd, connfd, clientlen, droppedConnFD;
    ThreadPool threadPool;
    MessageQueue connectionsQueue;
    Message connectionMessage;
    MessageMetaData messageMetaData;
    Content connectionMessageContent;
    int dropSize;// todo: set drop Size as per schedAlgo:in case of queue size is:0,Rand:((int)(0.25*queueSize))),and 1 otherwise
    int *droppedConnectionsContents;// todo: set MEM failure procedure +  Handling Errors procedure
    int droppedAmount;
    MQRetCode putRetCode;
    struct sockaddr_in clientaddr;

    if (initServerDataStructures(&threadPool, threadPoolSize, schedAlgo, &connectionsQueue, queueSize)) {
        initWorkerThreads(threadPool, connectionsQueue);

        TPSignalStartAll(threadPool);

        listenfd = Open_listenfd(port);

        while (1) {
            droppedAmount = 0;
            clientlen = sizeof(clientaddr);
            connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

            connectionMessageContent.fd = connfd;

            messageMetaData = buildMessageMetaData();

            if (!messageMetaData) {
                log("server.c: buildMessageMetaData failed\n");
                break;
            }

            messageMetaData->arrivalTime = getCurrentTime();
            connectionMessage = MessageCreate(connectionMessageContent, messageMetaData);

            // todo: wtf?
            droppedConnectionsContents = (int*)malloc(sizeof(int)*((int)(0.25*queueSize)));
            putRetCode = MQPut(connectionsQueue, connectionMessage, droppedConnectionsContents, &droppedAmount);

            MessageFree(connectionMessage);

            if (putRetCode == MQ_DROP) {
                for (int i = 0; i < droppedAmount; ++i) {
                    droppedConnFD = droppedConnectionsContents[i];

                    if (IS_DEBUG) {
                        printf("server.c: closing: %d\n", droppedConnFD);
                    }

                    Close(droppedConnFD);
                }
////Jus
                free(droppedConnectionsContents);
            } else if (putRetCode == MQ_SUCCESS) {
                free(droppedConnectionsContents);
                continue;
            } else {
                free(droppedConnectionsContents);
                log("server.c: MQPut failed.\n");
                break;   // todo: checking Handling Errors procedure
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int port, threadPoolSize, queueSize;
    char *schedAlgo = (char *) malloc(SCHED_ALG_MAX_SIZE);

    getargs(&port, &threadPoolSize, &queueSize, &schedAlgo, argc, argv);

    if (IS_DEBUG) {
        fprintf(stdout, "port: %d\n", port);
        fprintf(stdout, "thread pool size: %d\n", threadPoolSize);
        fprintf(stdout, "queue size: %d\n", queueSize);
        fprintf(stdout, "sched-algo: %s\n", schedAlgo);
    }

    return startServer(port, threadPoolSize, queueSize, schedAlgo);
}


    


 
