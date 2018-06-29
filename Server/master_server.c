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
    char fullMsg[MAX_BUFFER];

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
            fprintf(stderr, "\n[" ANSI_COLOR_MAGENTA "queue" ANSI_COLOR_RESET "] Pushing message to queue - socket %d\n", clientSocketFd);

            buildMessage(fullMsg, msgBuffer, clientSocketFd);

            queuePush(q, fullMsg);
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
        char* fullMsg = queuePop(q);
        pthread_mutex_unlock(q->mutex);
        pthread_cond_signal(q->notFull);

        //Prepare messsage to send client
        fprintf(stderr, "[" ANSI_COLOR_YELLOW "wait" ANSI_COLOR_RESET "] Prepare message!\n");

        //for(int i = 0; i < chatData->numClients; i++)
        //{
            fprintf(stderr, "[" ANSI_COLOR_BLUE "split" ANSI_COLOR_RESET "] Split message!\n");            

            fprintf(stderr, "[" ANSI_COLOR_YELLOW "fullmsg" ANSI_COLOR_RESET "] %s\n", fullMsg);
            
            int socket = splitBuffer(fullMsg);

            fprintf(stderr, "[" ANSI_COLOR_GREEN "send" ANSI_COLOR_RESET "] Send message!\n");
            
            if(socket != 0 && write(socket, fullMsg, MAX_BUFFER - 1) == -1)
                perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Socket write failed: ");
        //}
    }
}

void buildMessage(char *fullMsg, char *msgBuffer, int clientSocketFd){

    memset(fullMsg, 0, MAX_BUFFER);
    sprintf(fullMsg, "%d", clientSocketFd);
    strcat(fullMsg, ":");
    strcat(fullMsg, msgBuffer);
}

int splitBuffer(char *fullMsg){

    char **msg;
    char delimiters[] = ":";    
    char *token, *splitMsg;
    int clientFd;

    memcpy(splitMsg, fullMsg, MAX_BUFFER);
    
    token = strsep(&splitMsg, delimiters);
    printf("socket: %s\n", token);

    clientFd = atoi(token);
       
    token = strsep(&splitMsg, delimiters);
    printf("Hostname: %s\n", token);

    token = strsep(&splitMsg, delimiters);
    printf("Message: %s\n", token);
    
    if (strlen(token) > 1){
        memcpy(msg, &token,  sizeof(token));
        exeAction(msg[0]);
    }

    return clientFd;
}


command *splitMsg(char *msg){
    char *token;
    char delimiters[] = " ";
    command *c = (command*)malloc(sizeof(command));
    
    token = strsep(&msg, delimiters);
    c->com = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(c->com, token);

    token = strsep(&msg, delimiters);
    if (token != NULL){
        c->msg = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(c->msg, token);
    }else{
        c->msg = NULL;
    }
    return c;
}

void exeAction(char *msg){
    char *name;
    int resp;
    char *theMsg;
    
    pthread_t tId;
    pthread_mutex_init(&lock_pipe, NULL);

    strtok(msg, "\n");
    command *c;

    if(!strcmp(msg, "ls")){
        showDir(NULL);
        return;
    }


    c = splitMsg(msg);
    if(c->msg != NULL){
        if(!strcmp(c->com, "mkdir")){    
        
            pthread_mutex_lock(&lock_pipe);
        
            printf("wait for read...");
        
            pthread_create(&tId, NULL, pipeWriter, (void *) c->msg);
            pthread_join(tId, NULL);

            pthread_mutex_unlock(&lock_pipe);
        }       


        if(!strcmp(c->com, "mkfile")){    
            resp = createFile(c->msg);
        }      
        if(!strcmp(c->com, "rmfolder")){    
            resp = deleteFolder(c->msg);
        }
        if(!strcmp(c->com, "rmfile")){    
            resp = deleteFile(c->msg);
        }
        if(!strcmp(c->com, "cpfile")){    
            //resp = copyFile(c->msg);
        }         

    }else{
        free(c);
        return;
    }
    free(c);
}


void *pipeWriter(void *str){

    int fd;
    char *myFifo = "/tmp/myfifo";
    char *strPipe;

    strPipe = str;

    /* remove the Fifo if it already exist */  
    unlink(myFifo);

    /* create the FIFO (named pipe) */
    mkfifo(myFifo, 0666);

    /* write to the FIFO */
    fd = open(myFifo, O_WRONLY);
    write(fd, strPipe, 50);
    close(fd);

    /* remove the FIFO */
    unlink(myFifo);
}