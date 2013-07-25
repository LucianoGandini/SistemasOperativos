#include <stdio.h>      
#include <assert.h>     


int main(int argc, char *argv[])
{
	FILE * file = fopen("output","r");
	char buf[1024];
	
	while(!feof(file))
	{
		fgets(buf, 1024,file);
		assert(todosIguales(buf)); 
	}
	
	
	return 0;
}
