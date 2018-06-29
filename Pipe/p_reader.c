#include "p_reader.h"
#include<pthread.h>
#include<time.h>
#include <string.h>

void *pipeReader(){
    
    int fd;
    char *myFifo = "/tmp/myfifo";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
        fd = open(myFifo, O_RDONLY);
        read(fd, buffer, 1024);
        printf("[+] Received: %s\n", buffer);
        memset(buffer,0,strlen(buffer));
        close(fd);

}


int main(int argc, char const *argv[])
{
    pthread_t tId;
    while(1){
        pthread_create(&tId, NULL, pipeReader, NULL);
        sleep(3);
    	pthread_join(tId, NULL);
    }
    //pipeReader();
    return 0;
}
