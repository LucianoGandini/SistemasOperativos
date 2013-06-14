#ifndef RWLock2012_h
#define RWLock2012_h
#include <vector>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TIPO_LECTURA 0
#define TIPO_ESCRITURA 1

struct Peticion {
	pthread_mutex_t *mutex_peticion;
	int tipo_peticion;
};

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();
    private:
		int lectores_corriendo;
		int escritores_corriendo;
		std::queue<Peticion*> cola_peticiones;
		pthread_mutex_t mutex_cola_peticiones;
		pthread_mutex_t mutex_lectores_corriendo;
		pthread_mutex_t mutex_escritores_corriendo;
};

#endif
