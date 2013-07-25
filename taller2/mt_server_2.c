#include "mt_2.h"
int main()
{
	int sock, conn_fd, n;
	FILE * file;
	struct sockaddr_in name, cliaddr;
	socklen_t clilen;
	char buf[MAX_MSG_LENGTH];
	char bufClient[MAX_MSG_LENGTH * 10] = "";
	/* Crear socket sobre el que se lee: dominio INET, protocolo TCP (STREAM). */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		perror("error abriendo socket");
		exit(1);
	}
	perror("socket abierto");
	/* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(PORT);
	if (bind(sock, (void*) &name, sizeof(name))) 
	{
		perror("error binding datagram socket");
		exit(1);
	}			
	perror("socket bindeado");

	
	if (listen(sock, 1) == -1) {
        perror("error escuchando");
        exit(1);
    }
	perror("ya listenie");
	
	perror("se viene el acceeeeeeeept");
	conn_fd = accept(sock,(struct sockaddr *)&cliaddr,&clilen);
	perror("ya acceptie");
	
	
	char command[MAX_MSG_LENGTH] = "";
	char error[MAX_MSG_LENGTH] = " 2>&1";
	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) 
	{
		
		n = recv(conn_fd, buf, MAX_MSG_LENGTH, 0);
		perror("ya hice recv");
		if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
			break;
		replaceChar(buf,'\n','\0'); 
		join(command, buf);
		join(command, error);
		perror(command);
		
		
		
		file = popen(command,"r");
		command[0] = '\0';
		buf[0] = '\0';
		
		if (file == NULL)
		{
			perror("error ejecutando comando");
			exit(1);
		}
		
		while(!feof(file))
		{
			fgets(buf, MAX_MSG_LENGTH,file);
			join(bufClient,buf); 
		}
		
		send(conn_fd,bufClient,MAX_MSG_LENGTH,0);
		bufClient[0] = '\0';
		  
	}
	/* Cerrar socket de recepción. */
	close(sock);
	return 0;
}


void join(char s1[], char s2[])
{
	int i,j;
    for(i=0; s1[i]!='\0'; ++i); 
    for(j=0; s2[j]!='\0'; ++j, ++i) 
    { 
		s1[i]=s2[j]; 
	} 
	s1[i]='\0';
}

void replaceChar(char s[], char target, char replacement)
{
	int i;
	for(i=0; s[i]!=target; ++i);
	s[i] = replacement;
}

