#include "menu.h"

int menuAction(){
    
    int choice = 0;
        system("clear");
        printf("|          MENU        |\n\n");
        printf("| 1.  Create Partition |\n");
        printf("| 2.  Create Folder    |\n");
        printf("| 3.  Create File      |\n");
        printf("| 4.  Delete Partition |\n");
        printf("| 5.  Delete Folder    |\n");
        printf("| 6.  Delete File      |\n");
        printf("| 7.  Copy Folder      |\n");
        printf("| 8.  Copy File        |\n");
        printf("| 9.  List Directory   |\n");
        printf("| 10. Navigate         |\n");
        printf("> ");
        scanf("%d",&choice);

        if(choice == 1){

            return choice;
        }
        else if(choice == 2){
            printf("[+] Folder Name:\n");
            printf("> ");
            
            return choice;
        }
        else if(choice == 3){
            printf("[+] File Name:\n");
            printf("> ");

            return choice;
        }
        else if(choice == 4){
            printf("> Delete Partition\n");

            return choice;
        }
        else if(choice == 5){
            printf("> Delete Folder\n");

            return choice;
        }
        else if(choice == 6){
            printf("> Delete File\n");

            return choice;
        }
        else if(choice == 7){
            printf("> Copy Folder\n");

            return choice;
        }
        else if(choice == 8){
            printf("> Copy File\n");

            return choice;
        }
        else if(choice == 9){
            printf("> List Directory\n");

            return choice;
        }
        else if(choice == 10){
            printf("> You are in the folder x\n");

            return choice;
        }
        else {
            printf("> This option does not exist!\n");
        }
}