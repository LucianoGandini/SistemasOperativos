#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "RWLock.h"

#define  CANT_THREADS  300
#define TAMANIO_ARREGLO 100


int arreglo[TAMANIO_ARREGLO]; 
int thread_numero = 0;
bool hubo_lectores_simultaneos = false;
bool alguien_leyendo = false;
bool alguien_escribiendo = false;
RWLock rw;
RWLock lockALeyendo;
RWLock lockAEscribiendo;
pthread_mutex_t super_mutex;


void *leer_entero(void*){	
	rw.rlock();
	
	lockAEscribiendo.rlock();
	assert(!alguien_escribiendo);
	lockAEscribiendo.runlock();
	
	lockALeyendo.wlock();
	if (alguien_leyendo) hubo_lectores_simultaneos = true;
	alguien_leyendo = true; 
	lockALeyendo.wunlock();
	
	int i = rand() % TAMANIO_ARREGLO;
	printf("leyendo_entero = %d \n", arreglo[i]);
	
	lockALeyendo.wlock();
	alguien_leyendo = false; 
	lockALeyendo.wunlock();
	
	rw.runlock();
	return NULL;
}

void *escribir_entero(void*){
	rw.wlock();
	
	lockAEscribiendo.rlock();
	assert(!alguien_escribiendo);
	lockAEscribiendo.runlock();
	
	lockALeyendo.rlock();
	assert(!alguien_leyendo);
	lockALeyendo.runlock();
	
	int i = rand() % TAMANIO_ARREGLO;

	printf("escribiendo_entero = %d \n", i);
	arreglo[i] += 1;
	rw.wunlock();	
	return NULL;
}
int main (){

	pthread_t thread[CANT_THREADS];
    int tid;
    for (int i = 0; i < TAMANIO_ARREGLO; i++) arreglo[i] = 0;
	printf("threads_a_correr\n");
    for (tid = 0; tid < CANT_THREADS; ++tid) {
         if (((tid*9)&(0x3))==0){
			assert(pthread_create(&thread[tid], NULL, escribir_entero , NULL) == 0);
		}
         else{
			assert(pthread_create(&thread[tid], NULL, leer_entero, NULL) == 0);
		}
    }

    for (tid = 0; tid < CANT_THREADS; ++tid)
         pthread_join(thread[tid], NULL);
         
    int sumatoria = 0;
    
    for (int j = 0; j < TAMANIO_ARREGLO; j++) sumatoria += arreglo[j];
    
    assert(sumatoria  == CANT_THREADS/4);
    assert(hubo_lectores_simultaneos);   
	return 0;
}
