//
//  server.c
//  Batalla Naval
//
//  Created by Federico Raimondo on 4/24/13.
//  Copyright (c) 2013 ar.dc.uba.so. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <modelo.h>
#include <decodificador.h>
#include <globales.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <vector>

#define MAX_MSG_LENGTH 4096
#define MAX_JUGADORES 100

/* Setea un socket como no bloqueante */
int no_bloqueante(int fd) {
    int flags;
    /* Toma los flags del fd y agrega O_NONBLOCK */
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1 )
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


/* Variables globales del server */
int sock;								// Socket donde se escuchan las conexiones entrantes
int s_controlador, s_in_controlador;						/* Socket de comunicación del controlador */
struct sockaddr_in name, remote;
struct sockaddr_in local, remote_controlador;	// Direcciones
char buf[MAX_MSG_LENGTH];				// Buffer de recepción de mensajes
char buf_controlador[MAX_MSG_LENGTH];	// Buffer de recepción de mensajes del controlador
int s[MAX_JUGADORES];					// Sockets de los jugadores
int ids[MAX_JUGADORES];					// Ids de los jugadores
Modelo * model = NULL;					// Puntero al modelo del juego
Decodificador *decoder  = NULL;			// Puntero al decodificador
int n, tamanio, tamanio_barcos;			// Variables de configuracion del juego.
pthread_t thread_controlador;			// Thread para el controlador
vector<pthread_t> threads_controlador;	// Threads para las conexiones entrantes al Controlador
pthread_t threads_jugadores[MAX_JUGADORES];	// Threads de atencion para cada jugador

/* Resetea el juego */
void reset() {
	if (model != NULL) {
		delete model;
	}
	if (decoder != NULL) {
		delete decoder;
	}
	model = new Modelo(n, tamanio, tamanio_barcos);
	decoder = new Decodificador(model);
}

/* Acepta todas las conexiones entrantes */

// void accept() {
// 	int t;
// 	for (int i = 0; i < n; i++) {
// 		t = sizeof(remote);
// 		if ((s[i] = accept(sock, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
// 			perror("aceptando la conexión entrante");
// 			exit(1);
// 		}
// 		ids[i] = -1;
// 		int flag = 1;
// 		setsockopt(s[i],            /* socket affected */
// 				IPPROTO_TCP,     /* set option at TCP level */
// 				TCP_NODELAY,     /* name of option */
// 				(char *) &flag,  /* the cast is historical */
// 				sizeof(int));    /* length of option value */
// 	}
// }

/* Puerto de comunicación del controlador */
int port_controlador;
/* Para anteder al controlador */
void* atender_controlador(void *ptr) {
	
	// crear un socket de tipo INET con TCP (SOCK_STREAM)
	if ((s_controlador = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Error creando socket" << std::endl;
	}
	// permito reusar el socket para que no tire el error "Address Already in Use"
	//int flag = 1;
	//setsockopt(s_controlador, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

	// crear nombre, usamos INADDR_ANY para indicar que cualquiera puede conectarse aquí
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(port_controlador);
	if (bind(s_controlador, (const struct sockaddr *)&local, sizeof(local)) == -1) {
		std::cerr << "Error haciendo bind! " << errno << std::endl;
       exit(1);
	}
	
	/* Escuchar en el socket y permitir n conexiones en espera. */
	if (listen(s_controlador, n) == -1) {
		perror("escuchando");
		exit(1);
	}
	
	int recibido = -1;
	std::string resp;
	while(recibido != 0){
		
		int t = sizeof(remote_controlador);
		s_in_controlador = accept(s_controlador, (struct sockaddr*) &remote_controlador, (socklen_t*) &t); //TODO setear opciones del socket (si es necesario)
		std::cout << "accepted" << std::endl;
		assert(s_in_controlador >= 0);
		
		//recibido = recv(s[i], buf, MAX_MSG_LENGTH, 0);
		recibido = recv(s_in_controlador, buf_controlador, MAX_MSG_LENGTH, 0);
		
		std::cout << "2: " << recibido << std::endl;
		
		if (recibido < 0) {
			perror("Controlador::Recibiendo ");		
		} else if (recibido > 0) {
			std::cout << "viva asdf" << std::endl;
			buf_controlador[recibido]='\0';
			char * pch = strtok(buf_controlador, "|");
			while (pch != NULL) {
		
				//Ejecutar y responder
				resp = decoder->decodificar(pch);
				std::cout << "antes del send" << std::endl;
				send(s_controlador,resp.c_str(), resp.length() +1, 0);
				std::cout << "despues del send" << std::endl;		
				pch = strtok(NULL, "|");
			}
			

		}
		recibido = -1;	
	}
	std::cerr << "Controlador::The peer has performed an orderly shutdown" << std::endl;
	return NULL;
}


/* Para atender al i-esimo jugador */
void* atender_jugador(void * id) {
	long i = (long) id;
	int recibido = -1;//inicializo en -1 para poder entrar al while
	std::string resp;
	while( recibido != 0 ){
		recibido = recv(s[i], buf, MAX_MSG_LENGTH, 0);
		if (recibido < 0) {
			perror("AttJugador::Recibiendo ");
			
		} else if (recibido > 0) {
			buf[recibido]='\0';
			// Separo los mensajes por el caracter |
			char * pch = strtok(buf, "|");
			while (pch != NULL) {
				
				// No muestro por pantalla los NOP, son muchos
				if (strstr(pch, "Nop") == NULL) {
					printf("Recibido: %s\n", pch);
				}
				
				//Decodifico el mensaje y obtengo una respuesta
				resp = decoder->decodificar(pch);
				
				// Si no se cual es el ID de este jugador, trato de obtenerlo de la respuesta
				if (ids[i] == -1) {
					ids[i] = decoder->dameIdJugador(resp.c_str());
				}
				
				// Envio la respuesta
				send(s[i],resp.c_str(), resp.length() +1, 0);
				
				// No muestro por pantalla los NOP, son muchos
				if (strstr(pch, "Nop") == NULL) {
					printf("Resultado %s\n", resp.c_str());
				}
				
				// Si ya se cual es el jugador
				if (ids[i] != -1) {
					// Busco si hay eventos para enviar y los mando
					int eventos = model->hayEventos(ids[i]);
					if (eventos != 0) {
						printf("Agregando %d eventos\n", eventos);
					}
					for (int ev = 0; ev < eventos; ev++) {
						resp = decoder->encodeEvent(ids[i]);
						printf("Enviando evento %s", resp.c_str());
						send(s[i],resp.c_str(), resp.length() +1, 0);
					}
				}
				pch = strtok(NULL, "|");
			}
		}
	}
	return NULL;
}

/*
 * Recibe 4 parametros:
 * argv[1]: Puerto
 * argv[2]: Cantidad de jugadores (N)
 * argv[3]: Tamanio del tablero
 * argv[4]: Tamanio total de los barcos
 */
int main(int argc, char * argv[]) {
	if (argc < 5) {
		printf("Faltan parametros\n");
		printf("Se espera ./server puerto jugadores tamanio_tablero tamanio_barcos\n");
		exit(1);
	}
	int port = atoi(argv[1]);
	n = atoi(argv[2]);
	tamanio = atoi(argv[3]);
	tamanio_barcos = atoi(argv[4]);
	
	inicializar();
	port_controlador = CONTROLLER_PORT;
	
	/* Lanzamos un thread con la rutina de atencion del controlador */
	assert( pthread_create(&thread_controlador, NULL, atender_controlador, NULL) == 0);
	
	printf("Escuchando en el puerto %d - controlador en %d\n", port, port_controlador);
	printf("Jugadores %d - Tamanio %d - Tamanio Barcos %d\n", n, tamanio, tamanio_barcos);
	reset();

	
	
	/* Crear socket sobre el que se lee: dominio INET, protocolo TCP (STREAM). */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("abriendo socket");
		exit(1);
	}
	/* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(port);
	if (bind(sock, (const struct sockaddr*) &name, sizeof(name))) {
		perror("binding socket");
		exit(1);
	}
	
	/* Escuchar en el socket y permitir n conexiones en espera. */
	if (listen(sock, n) == -1) {
		perror("escuchando");
		exit(1);
	}

	int t;
	for (long i = 0; i < n; i++) {
		t = sizeof(remote);
		if ((s[i] = accept(sock, (struct sockaddr*) &remote, (socklen_t*) &t)) == -1) {
			perror("aceptando la conexión entrante");
			exit(1);
		}
		ids[i] = -1;
		int flag = 1;
		setsockopt(s[i],            /* socket affected */
				IPPROTO_TCP,     /* set option at TCP level */
				TCP_NODELAY,     /* name of option */
				(char *) &flag,  /* the cast is historical */
				sizeof(int));    /* length of option value */
		
		if (DEBUGEAR) printf("Server::lanzando thread atender_jugador(%ld) \n", i);
		assert( pthread_create(&threads_jugadores[i],NULL,atender_jugador,(void*)i) == 0);
	}

	
	printf("Corriendo...\n");
	
	// bool sale = false;
	// while (!sale) {
	// 	fd_set readfds;
	// 	FD_ZERO(&readfds);
	// 	for (int i = 0; i < n; i++) {
	// 		FD_SET(s[i], &readfds);
	// 	}
	// 	select(s[n-1]+1, &readfds, NULL, NULL, NULL);
		
	// 	for (int i = 0; i < n; i++) {
	// 		if (FD_ISSET(s[i], &readfds)) {
	// 			atender_jugador(i);
	// 		}
	// 	}
		
	// }

	for (int i = 0; i < n; i++) {
		pthread_join(threads_jugadores[i], NULL);
		close(s[i]);
	}

	close(sock);
	if (pthread_cancel(thread_controlador) != 0) {
		std::cerr << "Error cerrando thread del controlador" << std::endl;
	}//TODO hacer join?
	return 0;
}
