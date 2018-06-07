#include "master_server.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr; //Basic structures for all syscalls and functions that deal with internet addresses
    long port = DEFAULT_PORT; //Set default port
    int socketFd; //Socket descriptor

    if(argc == 2) port = strtol(argv[1], NULL, 0); //If argument is NULL port is DEFAULT_PORT

    //SOCKET
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Socket creation failed");
        exit(1);
    }

    //BIND
    bindSocket(&serverAddr, socketFd, port);
    if(listen(socketFd, 1) == -1)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "]Listen failed: ");
        exit(1);
    }

    startConn(socketFd);
    
    close(socketFd);
}

//Spawns the new client handler thread and message consumer thread
void startConn(int socketFd)
{
    connDataVars data;
    data.numClients = 0;
    data.socketFd = socketFd;
    data.queue = queueInit();
    data.clientListMutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(data.clientListMutex, NULL);

    //Start thread to handle new client connections
    fprintf(stderr,"[" ANSI_COLOR_YELLOW "wait" ANSI_COLOR_RESET "] Connection handler\n");
    sleep(WAIT_TIME);
    pthread_t connectionThread;
    if((pthread_create(&connectionThread, NULL, (void *)&newClientHandler, (void *)&data)) == 0)
    {
        fprintf(stderr,"[" ANSI_COLOR_BLUE "thread" ANSI_COLOR_RESET "] Thread created for connection handler\n");
        fprintf(stderr,"[" ANSI_COLOR_GREEN "start" ANSI_COLOR_RESET "] Connection handler\n");
    }

    FD_ZERO(&(data.serverReadFds));
    FD_SET(socketFd, &(data.serverReadFds));

    //Start thread to handle messages received
    fprintf(stderr, "[" ANSI_COLOR_YELLOW "wait" ANSI_COLOR_RESET "] Message handler started\n");
    sleep(WAIT_TIME);
    pthread_t messagesThread;
    if((pthread_create(&messagesThread, NULL, (void *)&messageHandler, (void *)&data)) == 0)
    {
        fprintf(stderr, "[" ANSI_COLOR_BLUE "thread" ANSI_COLOR_RESET "] Thread created for message handler\n");
        fprintf(stderr, "[" ANSI_COLOR_GREEN "start" ANSI_COLOR_RESET "] Message handler\n");
        fprintf(stderr,"[" ANSI_COLOR_YELLOW "wait" ANSI_COLOR_RESET "] Wait for connections\n");
    }

    pthread_join(connectionThread, NULL);
    pthread_join(messagesThread, NULL);

    queueDestroy(data.queue);
    pthread_mutex_destroy(data.clientListMutex);
    free(data.clientListMutex);
}

//Initializes queue
queue* queueInit(void)
{
    queue *q = (queue *)malloc(sizeof(queue));
    if(q == NULL)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);
    }

    q->empty = 1;
    q->full = q->head = q->tail = 0;
    q->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if(q->mutex == NULL)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(q->mutex, NULL);

    q->notFull = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    if(q->notFull == NULL)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);   
    }
    pthread_cond_init(q->notFull, NULL);

    q->notEmpty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    if(q->notEmpty == NULL)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);
    }
    pthread_cond_init(q->notEmpty, NULL);

    return q;
}

//Frees a queue
void queueDestroy(queue *q)
{
    pthread_mutex_destroy(q->mutex);
    pthread_cond_destroy(q->notFull);
    pthread_cond_destroy(q->notEmpty);
    free(q->mutex);
    free(q->notFull);
    free(q->notEmpty);
    free(q);
    printf("[" ANSI_COLOR_BLUE "error" ANSI_COLOR_RESET "] Clean queue!\n");
}

//Push to end of queue
void queuePush(queue *q, char* msg)
{
    q->buffer[q->tail] = msg;
    q->tail++;
    if(q->tail == MAX_BUFFER)
        q->tail = 0;
    if(q->tail == q->head)
        q->full = 1;
    q->empty = 0;
}

//Pop front of queue
char* queuePop(queue *q)
{
    char* msg = q->buffer[q->head];
    q->head++;
    if(q->head == MAX_BUFFER)
        q->head = 0;
    if(q->head == q->tail)
        q->empty = 1;
    q->full = 0;

    return msg;
}

//Sets up and binds the socket
void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port)
{
    memset(serverAddr, 0, sizeof(*serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr->sin_port = htons(port);

    if(bind(socketFd, (struct sockaddr *)serverAddr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Socket bind failed: ");
        exit(1);
    }
}

//Removes the socket from the list of active client sockets and closes it
void removeClient(connDataVars *data, int clientSocketFd)
{
    pthread_mutex_lock(data->clientListMutex);
    for(int i = 0; i < MAX_BUFFER; i++)
    {
        if(data->clientSockets[i] == clientSocketFd)
        {
            data->clientSockets[i] = 0;
            close(clientSocketFd);
            data->numClients--;
            i = MAX_BUFFER;
        }
    }
    pthread_mutex_unlock(data->clientListMutex);
}

//Thread to handle new connections. Adds client's fd to list of client fds and spawns a new clientHandler thread for it
void *newClientHandler(void *data)
{
    connDataVars *chatData = (connDataVars *) data;
    while(1)
    {
        int clientSocketFd = accept(chatData->socketFd, NULL, NULL);
        if(clientSocketFd > 0)
        {
            fprintf(stderr,"[" ANSI_COLOR_GREEN "connect" ANSI_COLOR_RESET "] Server accepted new client in socket %d!\n", clientSocketFd);

            //Obtain lock on clients list and add new client in
            pthread_mutex_lock(chatData->clientListMutex);
            if(chatData->numClients < MAX_BUFFER)
            {
                //Add new client to list
                for(int i = 0; i < MAX_BUFFER; i++)
                {
                    if(!FD_ISSET(chatData->clientSockets[i], &(chatData->serverReadFds)))
                    {
                        chatData->clientSockets[i] = clientSocketFd;
                        i = MAX_BUFFER;
                    }
                }

                FD_SET(clientSocketFd, &(chatData->serverReadFds));

                //Spawn new thread to handle client's messages
                clientHandlerVars chv;
                chv.clientSocketFd = clientSocketFd;
                chv.data = chatData;

                pthread_t clientThread;
                if((pthread_create(&clientThread, NULL, (void *)&clientHandler, (void *)&chv)) == 0)
                {
                    chatData->numClients++;
                    fprintf(stderr, "[" ANSI_COLOR_BLUE "thread" ANSI_COLOR_RESET "] Thread created for client connected in socket %d\n", clientSocketFd);
                }
                else
                    close(clientSocketFd);
            }
            pthread_mutex_unlock(chatData->clientListMutex);
        }
    }
}

//The "producer" -- Listens for messages from client to add to message queue
void *clientHandler(void *chv)
{
    clientHandlerVars *vars = (clientHandlerVars *)chv;
    connDataVars *data = (connDataVars *)vars->data;

    queue *q = data->queue;
    int clientSocketFd = vars->clientSocketFd;

    char msgBuffer[MAX_BUFFER];
    while(1)
    {
        int numBytesRead = read(clientSocketFd, msgBuffer, MAX_BUFFER - 1);
        msgBuffer[numBytesRead] = '\0';

        //If the client sent /exit\n, remove them from the client list and close their socket
        if(strcmp(msgBuffer, "/exit\n") == 0)
        {
            fprintf(stderr, "[" ANSI_COLOR_RED "disconnect" ANSI_COLOR_RESET"] Client on socket %d has disconnected!\n", clientSocketFd);
            removeClient(data, clientSocketFd);
            return NULL;
        }
        else
        {
            //Wait for queue to not be full before pushing message
            while(q->full)
            {
                pthread_cond_wait(q->notFull, q->mutex);
            }

            //Obtain lock, push message to queue, unlock, set condition variable
            pthread_mutex_lock(q->mutex);
            fprintf(stderr, "[" ANSI_COLOR_MAGENTA "queue" ANSI_COLOR_RESET "] Pushing message to queue\n> Message:%s> ClientSocket:%d\n", msgBuffer, clientSocketFd);

            myProtocol(msgBuffer, clientSocketFd);

            queuePush(q, msgBuffer);
            pthread_mutex_unlock(q->mutex);
            pthread_cond_signal(q->notEmpty);
        }
    }
}

//The "consumer" -- waits for the queue to have messages then takes them out and broadcasts to clients
void *messageHandler(void *data)
{   
    connDataVars *chatData = (connDataVars *)data;
    queue *q = chatData->queue;
    int *clientSockets = chatData->clientSockets;

    while(1)
    {
        //Obtain lock and pop message from queue when not empty
        pthread_mutex_lock(q->mutex);
        while(q->empty)
        {
            pthread_cond_wait(q->notEmpty, q->mutex);
        }
        char* msg = queuePop(q);
        pthread_mutex_unlock(q->mutex);
        pthread_cond_signal(q->notFull);


        exe(bBuffer(msg));
        //Broadcast message to all connected replicated servers
        fprintf(stderr, "[" ANSI_COLOR_GREEN "message" ANSI_COLOR_RESET "] Broadcast to replicated servers!\n> %s\n", msg);

        for(int i = 0; i < chatData->numClients; i++)
        {
            int socket = clientSockets[0];
           if(socket != 0 && write(socket, msg, MAX_BUFFER - 1) == -1)
                perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Socket write failed: ");
        }
    }
}