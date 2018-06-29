#include "p_reader.h"
#include <pthread.h>
#include <time.h>
#include <string.h>

void *pipeReaderDir(){
    
    int fd;
    char *myFifo = "/tmp/pipeDir";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
        fd = open(myFifo, O_RDONLY);
        read(fd, buffer, 1024);
        if(strlen(buffer) != 0){
            printf("[+] Path: %s\n", buffer);
            memset(buffer,0,strlen(buffer));
            close(fd);
        }
}
void *pipeReaderFile(){
    
    int fd;
    char *myFifo = "/tmp/pipeFile";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
        fd = open(myFifo, O_RDONLY);
        read(fd, buffer, 1024);
        if(strlen(buffer) != 0){
            printf("[+] Path/File: %s\n", buffer);
            memset(buffer,0,strlen(buffer));
            close(fd);
        }
}


int main(int argc, char const *argv[])
{
    pthread_t tId[2];
    while(1){
        pthread_create(&tId[0], NULL, pipeReaderDir, NULL);
        pthread_create(&tId[1], NULL, pipeReaderFile, NULL);

    	pthread_join(tId[0], NULL);
    	pthread_join(tId[1], NULL);
    }

    return 0;
}
