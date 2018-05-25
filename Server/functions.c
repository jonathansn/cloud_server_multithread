#include "functions.h"

void execFunction(char *buffer){

    switch(buffer[0]){
        case 1 :
            //createFolder(&buffer[1]);
                break;
        case 2 :
            //createFile(&buffer[1]);
                break;
        case 3 :
            //copyFolder(&buffer[1], &buffer[50]);
             break;
        case 4 :
            //copyFile(&buffer[1], &buffer[50]);
                break;
        case 5 :
            //listDir(&buffer[1]);
                break;
        case 6 :
            //deleteFolder(&buffer[1]);
                break;
        case 7 :
            //deleteFile(&buffer[1]);
                break;
        default:    printf("This option does not exist!\n");
    }
}