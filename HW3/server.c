#include "segel.h"
#include "request.h"
#include "message_queue.h"
#include "thread_pool.h"
#include "logger.h"

#define SCHED_ALG_MAX_SIZE 7

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


void* threadHandleRequest(void *connectionsQueuePtr) {
    MessageQueue castedConnectionsQueue = (MessageQueue) connectionsQueuePtr;
    Message currentConnectionMessage;
    MQRetCode getRetCode;
    int currentConnFd;

    log("threadHandleRequest: start\n");

    while (1) {
        getRetCode = MQGet(castedConnectionsQueue, &currentConnectionMessage);

        if (getRetCode == MQ_SUCCESS) {
            currentConnFd = currentConnectionMessage->content.fd;
            requestHandle(currentConnFd);
            Close(currentConnFd);
        } else {
            log("\"threadHandleRequest: error with MQGet \n");
            break;
        }

        log("threadHandleRequest: iteration done\n");
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
    for (int i = 0; i < TPGetPoolSize(threadPool); i++) {
        TPAddThread(threadPool, threadHandleRequest, connectionsQueue);
    }
}

int startServer(int port, int threadPoolSize, int queueSize, char *schedAlgo) {
    int listenfd, connfd, clientlen, droppedConnFD;
    ThreadPool threadPool;
    MessageQueue connectionsQueue;
    Message connectionMessage;
    Content connectionMessageContent;
    Content droppedConnectionContent;
    MQRetCode putRetCode;
    struct sockaddr_in clientaddr;

    if (initServerDataStructures(&threadPool, threadPoolSize, schedAlgo, &connectionsQueue, queueSize)) {
        initWorkerThreads(threadPool, connectionsQueue);

        TPSignalStartAll(threadPool);

        listenfd = Open_listenfd(port);

        while (1) {
            clientlen = sizeof(clientaddr);
            connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

            connectionMessageContent.fd = connfd;
            connectionMessage = MessageCreate(connectionMessageContent, MSG_INT);

            putRetCode = MQPut(connectionsQueue, connectionMessage, &droppedConnectionContent);

            MessageFree(connectionMessage);

            if (putRetCode == MQ_DROP) {
                droppedConnFD = droppedConnectionContent.fd;

                // todo: remove
                printf("server.c: closing: %d\n", droppedConnFD);

                Close(droppedConnFD);
            } else if (putRetCode == MQ_SUCCESS) {
                continue;
            } else {
                log("server.c: MQPUT failed.\n");
                break;
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int port, threadPoolSize, queueSize; // todo: make unsigned?
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


    


 
