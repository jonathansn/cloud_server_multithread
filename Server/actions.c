#include "actions.h"

char *createFolder(char str[]){
	char *ret;
	pid_t childpid;
	childpid=fork();

	strcat(ret, "Folder ");
	strcat(ret, str);

	if(childpid == -1){
		perror("Fork failed");
	}
	if(childpid == 0){   
		if(showDir(str)){ 
			printf("Folder %s already exists!\n", str);
			strcat(ret," already exists!");
			return ret;
		}else{
			printf("Folder %s created!\n", str);
			strcat(ret," created!");
			execl("/bin/mkdir", "mkdir", str, NULL);
			perror("Child failed to exec");
		}
	}

	//printf("TESTE antes do return\n");
	return ret;
}

int createFile (char str[]){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return 1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 
			printf("File %s already exists!\n", str);
		}else{
			printf("File %s created!\n", str);
			execl("/usr/bin/touch", "touch", str, NULL);
			perror("Child failed to exec");
			return 1;
		}
	}
	return EXIT_FAILURE;
}


int showDir(char str[]){
	struct dirent **namelist;
	int n;

	n = scandir(".", &namelist, NULL, alphasort);
	
	if (n == -1) {
	   	perror("scandir");
	   	exit(EXIT_FAILURE);
	}
	
	if (str == NULL){
		while (n--) {
			printf("%s\n", namelist[n]->d_name);
		   	free(namelist[n]);
		}
		free(namelist);
	}else{
		while (n--) {
			if(strcmp(namelist[n]->d_name, str) == 0){
				return 1;
			}
		   	free(namelist[n]);
		}
		free(namelist);
	}
}

int deleteFolder(char str[]){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return 1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 			
			printf("Folder %s deleted!\n", str);
			execl("/usr/bin/rm", "rm", "-rf", str, NULL);
			perror("Child failed to exec");
		}else{
			printf("Folder %s not found\n", str);
			return 1;
		}
	}
	return EXIT_FAILURE;
}




int deleteFile(char str[]){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return 1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 
			printf("File %s deleted!\n", str);
			execl("/usr/bin/rm", "rm", str, NULL);
			perror("Child failed to exec");
		}else{
			printf("File %s not found\n", str);
			
			return 1;
		}
	}
	return EXIT_FAILURE;
}

int copyFile(char *source, char *destiny){
	execl("/usr/bin/cp", "cp", &source, &destiny, NULL);
}