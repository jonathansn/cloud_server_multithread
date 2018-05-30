#include "functions.h"

void exeFunction(char *buffer){
    		
    if(buffer[0] == 2){

        printf("[+] Buffer: %d %s\n",buffer[0] ,&buffer[1]);
        printf("[+] Action: %d\n",buffer[0]);
        printf("[+] Folder: %s\n",&buffer[1]);
        
        make_folder(&buffer[1]);
    }
    if(buffer[0] == 3){

        printf("[+] Buffer: %d %s\n",buffer[0] ,&buffer[1]);
        printf("[+] Action: %d\n",buffer[0]);
        printf("[+] File  : %s\n",&buffer[1]);
        
        make_file(&buffer[1]);
    }

    if(buffer[0] == 5){

        printf("[+] Buffer: %d %s\n",buffer[0] ,&buffer[1]);
        printf("[+] Action: %d\n",buffer[0]);
        printf("[+] File  : %s\n",&buffer[1]);
        
        remove_folder(&buffer[1]);
    }

    if(buffer[0] == 6){

        printf("[+] Buffer: %d %s\n",buffer[0] ,&buffer[1]);
        printf("[+] Action: %d\n",buffer[0]);
        printf("[+] File  : %s\n",&buffer[1]);
        
        remove_file(&buffer[1]);
    }

    if(buffer[0] == 9){

        printf("[+] Buffer: %d %s\n",buffer[0] ,&buffer[1]);
        printf("[+] Action: %d\n",buffer[0]);
        printf("[+] File  : %s\n",&buffer[1]);
        
        list_dir(NULL);
    }
}

//break buffer
void bBuffer(char buffer[]){
    int i, n, p = 0, k=0;
    char *aux;
    
    n = strlen(buffer);
    
    printf("STRLEN %d\n", n);
    printf("BUFFER %s\n", buffer);
    //break hostname
    for (i = 0; i < n-1; i++){
        if(buffer[i] == ':'){
            p = i+1;
        } 
    }

    for(i = p; i < n; i++){
        aux[k++] = buffer[i];
        printf("%d teste if2 %c \n", i, aux[i]);    
    }
    

    
    printf("AUX %s\n", aux);    
}