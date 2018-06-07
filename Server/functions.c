#include "functions.h"


<<<<<<< HEAD

=======
//break buffer
char** bBuffer(char buffer[]){
    char *token, **m;
    int x = 0;
    printf("buffer = %s\n", buffer );
    token = strsep(&buffer, " ");

    while(token != NULL){
        m[x] = token;
        printf("m %s\n", m[x]);
        token = strsep(&buffer, " ");  
        x++;      
    }   
return m;
}

char* exe(char **buffer){
    if(!strcmp(buffer[0],"mkdir")){
        createFolder(&buffer[1][0]);
    }

return "error";
}
>>>>>>> afc5d668ae2471ef0347a0003e3cdfb93a34613d
