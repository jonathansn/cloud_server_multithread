#include "client.h"

void chatloop(char *hostname, int socketFd);
void buildMessage(char *result, char *hostname, char *msg);
void setupAndConnect(struct sockaddr_in *serverAddr, struct hostent *host, int socketFd, long port);
void setNonBlock(int fd);
void interruptHandler(int sig);

static int socketFd;

int main(int argc, char *argv[])
{
    char *hostname;
    struct sockaddr_in serverAddr;
    struct hostent *host;
    long port;

    if(argc != 4)
    {
        fprintf(stderr, "./client [hostname] [server_ip] [server_port]\n");
        exit(1);
    }
    hostname = argv[1];
    if((host = gethostbyname(argv[2])) == NULL)
    {
        fprintf(stderr, "[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't get host hostname\n");
        exit(1);
    }
    port = strtol(argv[3], NULL, 0);
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        fprintf(stderr, "[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't create socket\n");
        exit(1);
    }    

    setupAndConnect(&serverAddr, host, socketFd, port);
    setNonBlock(socketFd);
    setNonBlock(0);

    //Set a handler for the interrupt signal
    signal(SIGINT, interruptHandler);

    chatloop(hostname, socketFd);
}

//Main loop to take in chat input and display output
void chatloop(char *hostname, int socketFd)
{
    fd_set clientFds;
    char fullMsg[MAX_BUFFER];
    char chatBuffer[MAX_BUFFER], msgBuffer[MAX_BUFFER];

    while(1)
    {
        //Reset the fd set each time since select() modifies it
        FD_ZERO(&clientFds);
        FD_SET(socketFd, &clientFds);
        FD_SET(0, &clientFds);
        if(select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) != -1) //wait for an available fd
        {
            for(int fd = 0; fd < FD_SETSIZE; fd++)
            {
                if(FD_ISSET(fd, &clientFds))
                {
                    if(fd == socketFd) //receive data from server
                    {
                        int numBytesRead = read(socketFd, msgBuffer, MAX_BUFFER - 1);
                        msgBuffer[numBytesRead] = '\0';
                        printf("%s", msgBuffer);
                        memset(&msgBuffer, 0, sizeof(msgBuffer));
                    }
                    else if(fd == 0) //read from keyboard (stdin) and send to server
                    {
                        fgets(chatBuffer, MAX_BUFFER - 1, stdin);
                        if(strcmp(chatBuffer, "/exit\n") == 0)
                            interruptHandler(-1); //Reuse the interruptHandler function to disconnect the client
                        else
                        {
                            buildMessage(fullMsg, hostname, chatBuffer);
                            if(write(socketFd, fullMsg, MAX_BUFFER - 1) == -1) perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] write failed: ");
                            memset(&chatBuffer, 0, sizeof(chatBuffer));
                        }
                    }
                }
            }
        }
    }
}

//Concatenates the hostname with the message and puts it into result
void buildMessage(char *result, char *hostname, char *msg)
{
    memset(result, 0, MAX_BUFFER);
    strcpy(result, hostname);
    strcat(result, ":");
    strcat(result, msg);
}

//Sets up the socket and connects
void setupAndConnect(struct sockaddr_in *serverAddr, struct hostent *host, int socketFd, long port)
{
    memset(serverAddr, 0, sizeof(serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr = *((struct in_addr *)host->h_addr_list[0]);
    serverAddr->sin_port = htons(port);
    if(connect(socketFd, (struct sockaddr *) serverAddr, sizeof(struct sockaddr)) < 0)
    {
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] Couldn't connect to server");
        exit(1);
    }
}

//Sets the fd to nonblocking
void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] fcntl failed");

    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

//Notify the server when the client exits by sending "/exit"
void interruptHandler(int sig_unused)
{
    if(write(socketFd, "/exit\n", MAX_BUFFER - 1) == -1)
        perror("[" ANSI_COLOR_RED "error" ANSI_COLOR_RESET "] write failed: ");

    close(socketFd);
    exit(1);
}

void strSplit(){

}