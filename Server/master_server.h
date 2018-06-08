#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stddef.h>

#include "color.h"
#include "functions.h"

#define MAX_BUFFER 1024
#define DEFAULT_PORT 8000

#define WAIT_TIME 0

/*
Queue implementation using a char array.
Contains a mutex for functions to lock on before modifying the array,
and condition variables for when it's not empty or full.
*/
typedef struct {
    char *buffer[MAX_BUFFER];
    int head, tail;
    int full, empty;
    pthread_mutex_t *mutex;
    pthread_cond_t *notFull, *notEmpty;
} queue;

/*
Struct containing important data for the server to work.
Namely the list of client sockets, that list's mutex,
the server's socket for new connections, and the message queue
*/
typedef struct {
    fd_set serverReadFds;
    int socketFd;
    int clientSockets[MAX_BUFFER];
    int numClients;
    pthread_mutex_t *clientListMutex;
    queue *queue;
} connDataVars;

/*
Simple struct to hold the connDataVars and the new client's socket fd.
Used only in the client handler thread.
*/
typedef struct {
    connDataVars *data;
    int clientSocketFd;
} clientHandlerVars;

void startConn(int socketFd);
void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port);
void removeClient(connDataVars *data, int clientSocketFd);

void *newClientHandler(void *data);
void *clientHandler(void *chv);
void *messageHandler(void *data);

void queueDestroy(queue *q);
queue* queueInit(void);
void queuePush(queue *q, char* msg);
char* queuePop(queue *q);

void buildMessage(char *fullMsg, char *msgBuffer, int clientSocketFd);
int splitBuffer(char *fullMsg);

#endif