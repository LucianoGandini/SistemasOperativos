#include "client_2.h"



int main(int argc, char *argv[] )
{
    int s, len;
    struct sockaddr_in remote;
    char* ip = argv[1];
	int inet_atom_result = inet_aton(ip, &remote.sin_addr);
	if( inet_atom_result == 0){
		perror("horrible tu ip"); 
		exit(1);
	} 
	
    char str[MENSAJE_MAXIMO];

	/* Crear un socket de tipo UNIX con TCP (SOCK_STREAM). */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("creando socket ineeeeeeeeeeeeeeeet");
        exit(1);	
    }

	/* Establecer la dirección a la cual conectarse. */
    remote.sin_family = AF_INET;
    remote.sin_port = htons(5555);
	len = sizeof(remote);


   	/* Conectarse. */
    if (connect(s, (struct sockaddr *) &remote, len) == -1) {
        perror("conectandose");
        exit(1);
    }

	/* Establecer la dirección a la cual conectarse para escuchar. */
    while(printf("> "), fgets(str, MENSAJE_MAXIMO, stdin), !feof(stdin)) {
        if (sendto(s, str, strlen(str) + 1, 0, &remote, len) == -1) {
            perror("enviando");
            exit(1);
        }
    }

	/* Cerrar el socket. */
    close(s);

    return 0;
}



