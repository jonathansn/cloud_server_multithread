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
#include <sys/stat.h>


#include "color.h"
#include "actions.h"

#define MAX_BUFFER 1024
#define DEFAULT_PORT 8000

#define WAIT_TIME 0

pthread_mutex_t lock_pipeDir;
pthread_mutex_t lock_pipeFile;

/*
Implementação de fila usando uma matriz char.
Contém um mutex para funções a serem bloqueadas antes de modificar a matriz,
e variáveis de condição para quando não está vazio ou cheio.
*/
typedef struct {
    char *buffer[MAX_BUFFER];
    int head, tail;
    int full, empty;
    pthread_mutex_t *mutex;
    pthread_cond_t *notFull, *notEmpty;
} queue;

/*
Estrutura contendo dados importantes para o servidor funcionar.
Ou seja, a lista de sockets do cliente, o mutex dessa lista,
o soquete do servidor para novas conexões e a fila de mensagens
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
Estrutura simples para manter o connDataVars e o novo socket_fd do cliente.
Usado apenas no encadeamento do manipulador de clientes.
*/
typedef struct {
    connDataVars *data;
    int clientSocketFd;
} clientHandlerVars;


struct command {
    char *com;
    char *msg;
};
typedef struct command command;

int splitBuffer(char *fullMsg);
void exeAction(char *msg);


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


void *pipeWriterDir(void *str);
void *pipeWriterFile(void *str);
#endif
