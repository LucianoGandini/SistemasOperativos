#include <stdio.h>      
#include <assert.h>     


int main(int argc, char *argv[])
{
	FILE * file;
	char buf[1024];
	//char command[20] = argv[1];
	int todosIguales = 1;
	int i;

	file = popen(argv[1], "r"); 
	if (file == NULL)
	{
		perror("error ejecutando comando");
		exit(1);
	}
		
	while(!feof(file))
	{
		fgets(buf, 1024,file);
		printf("Output: %s ", buf);
		todosIguales = 1;

		for(i=1; buf[i] != '\n'; ++i)
		{
			if (buf[i] != buf[0]){
				todosIguales = 0;
				break;
			} 
		}
		
		assert(todosIguales); 
	}
	printf("Salio todo bien! yupi! \n");
	return 0;
}


