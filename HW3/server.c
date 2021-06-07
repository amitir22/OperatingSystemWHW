#include "segel.h"
#include "request.h"
#include "message_queue.h"

#define SCHED_ALG_MAX_SIZE 7

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void getargs(int *port, int *threadPoolSize, int *queueSize, char **schedAlg, int argc, char *argv[])
{
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


void threadHandleRequest(MessageQueue connectionsQueue) {
    int currentConnFd;
    Message currentConnectionMessage;
    MQRetCode getRetCode;

    while (1) {
        getRetCode = MQGet(connectionsQueue, &currentConnectionMessage);

        if (getRetCode == MQ_SUCCESS) {
            currentConnFd = currentConnectionMessage->content.fd;
            requestHandle(currentConnFd);
        }
    }
}

int main(int argc, char *argv[])
{
    MessageQueue connectionsQueue;
    Message connectionMessage;
    Content connectionMessageContent;
    MQRetCode putRetCode;

    int listenfd, connfd, clientlen;
    int port, threadPoolSize, queueSize; // todo: make unsigned?
    char *schedAlgo = (char *) malloc(SCHED_ALG_MAX_SIZE);

    struct sockaddr_in clientaddr;

    getargs(&port, &threadPoolSize, &queueSize, &schedAlgo, argc, argv);

    // todo: remove
    fprintf(stdout, "port: %d\n", port);
    fprintf(stdout, "thread pool size: %d\n", threadPoolSize);
    fprintf(stdout, "queue size: %d\n", queueSize);
    fprintf(stdout, "sched-algo: %s\n", schedAlgo);

    connectionsQueue = MQCreate(queueSize, MSG_INT);

    // 
    // HW3: Create some threads...
    //

    listenfd = Open_listenfd(port);

    while (1) {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        connectionMessageContent.fd = connfd;
	    connectionMessage = MessageCreate(connectionMessageContent, MSG_INT);

        putRetCode = MQPut(connectionsQueue, connectionMessage);

        if (putRetCode == MQ_SUCCESS) {
            continue;
        }

        // todo: handle putRetCode

	    //
	    // HW3: In general, don't handle the request in the main thread.
	    // Save the relevant info in a buffer and have one of the worker threads
	    // do the work.
	    //

	    // while(1) {
	    // connectionQueue.get(connfd);
	    requestHandle(connfd);
	    // }

	    Close(connfd);
    }
}


    


 
