#include "actions.h"



int createFolder(char str[]){
	printf("+++++++++++++++creats: %s ", str);

return 1;
}





// int createFolder(char str[]){

// 	pid_t childpid;
// 	childpid=fork();


// 	if(childpid == -1){
// 		perror("Fork failed");
// 		return -1;
// 	}
// 	if(childpid == 0){   
// 		if(showDir(str)){ 
// 			printf("Server side, Folder %s already exists!\n", str);
// 			return 0;
// 		}else{
// 			printf("Server side, Folder %s created!\n", str);
// 			execl("/bin/mkdir", "mkdir", str, NULL);
// 			perror("Child failed to exec");
// 		}
// 	}
// 	return 1;
// }

int createFile (char str[]){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return -1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 
			printf("File %s already exists!\n", str);
			return 0;
		}else{
			printf("File %s created!\n", str);
			execl("/usr/bin/touch", "touch", str, NULL);
			perror("Child failed to exec");

		}
	}
	return 1;
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
		return -1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 			
			printf("Folder %s deleted!\n", str);
			execl("/usr/bin/rm", "rm", "-rf", str, NULL);
			perror("Child failed to exec");
		}else{
			printf("Folder %s not found\n", str);
			return 0;
		}
	}
	return 1;
}


int deleteFile(char str[]){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return -1;
	}
	if(childpid == 0){   
		if(showDir(str)){ 
			printf("File %s deleted!\n", str);
			execl("/usr/bin/rm", "rm", str, NULL);
			perror("Child failed to exec");
		}else{
			printf("File %s not found\n", str);
			
			return 0;
		}
	}
	return 1;
}

int copyFile(char *source, char *destiny){
	pid_t childpid;
	childpid=fork();

	if(childpid == -1){
		perror("Fork failed");
		return -1;
	}
	if(childpid == 0){
		if(showDir(source)){ 
			execl("/usr/bin/cp", "cp", source, destiny, NULL);
		}else{
			printf("File %s not found\n", source);
			return 0;
		}
	}
	return 1;
}