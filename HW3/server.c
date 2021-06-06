#include "segel.h"
#include "request.h"

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


int main(int argc, char *argv[])
{
    int listenfd, connfd, clientlen;
    int port, threadPoolSize, queueSize;
    char *schedAlgo = (char *) malloc(SCHED_ALG_MAX_SIZE);

    struct sockaddr_in clientaddr;

    getargs(&port, &threadPoolSize, &queueSize, &schedAlgo, argc, argv);

    // 
    // HW3: Create some threads...
    //
    fprintf(stdout, "port: %d\n", port);
    fprintf(stdout, "thread pool size: %d\n", threadPoolSize);
    fprintf(stdout, "queue size: %d\n", queueSize);
    fprintf(stdout, "sched-algo: %s\n", schedAlgo);

    listenfd = Open_listenfd(port);

    while (1) {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	    //
	    // HW3: In general, don't handle the request in the main thread.
	    // Save the relevant info in a buffer and have one of the worker threads
	    // do the work.
	    //
	    requestHandle(connfd);

	    Close(connfd);
    }
}


    


 
