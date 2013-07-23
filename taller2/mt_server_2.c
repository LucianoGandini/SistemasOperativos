#include "mt_2.h"
int main()
{
	int sock, conn_fd, n;
	struct sockaddr_in name, cliaddr;
	socklen_t clilen;
	char buf[MAX_MSG_LENGTH];
	char bufClient[MAX_MSG_LENGTH];
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
	
	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) 
	{
		
		n = recv(conn_fd, buf, MAX_MSG_LENGTH, 0);
		perror("ya hice recv");
		if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
			break;
		buf[n] = '\0';
		printf("Comando: %s", buf);
		system(buf);
	}
	/* Cerrar socket de recepción. */
	close(sock);
	return 0;
}

