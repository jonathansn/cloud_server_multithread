#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	
	FILE *arq;
	int result;
	char Str[50];
	int a = 2;


	arq = fopen("disk.dat", "rb+");

	if (arq != NULL)
	{
		fseek( arq, 5, SEEK_SET );
		fputs("teste", arq);


		printf("Abriu\n");
		fclose(arq);


		return 0;
	}else{
	    printf("Problemas na abertura do arquivo\n");
	    return 0;
	}	
	
}