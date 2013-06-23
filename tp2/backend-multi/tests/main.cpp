#include <stdio.h>
#include "RWLock.h"

#define  CANT_THREADS  80

int entero_global = 0; 
int thread_numero = 0;
RWLock rw;
pthread_mutex_t super_mutex;
void *leer_entero(void*){

	int algo = 0;

	
	rw.rlock();

	while (algo <= 500000)
		algo++;

	printf("leyendo_entero_global = %d \n", entero_global);
	rw.runlock();

	
	return NULL;
}

void *escribir_entero(void*){
	int algo = 0;

	rw.wlock();
	entero_global++;
	while (algo <= 500000)
		algo++;
	printf("escribiendo_entero_global = %d \n", entero_global);
	rw.wunlock();
	
	return NULL;
}

int main (){

	pthread_t thread[CANT_THREADS];
    int tid;
	printf("threads_a_correr\n");
    for (tid = 0; tid < CANT_THREADS; ++tid) {
         if (((tid*9)&(0x3))==0)
			pthread_create(&thread[tid], NULL, escribir_entero , NULL);
         else
			pthread_create(&thread[tid], NULL, leer_entero, NULL);
    }

    for (tid = 0; tid < CANT_THREADS; ++tid)
         pthread_join(thread[tid], NULL);
         
        
	return 0;
}






