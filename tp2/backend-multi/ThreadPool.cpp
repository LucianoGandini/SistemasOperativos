#include "ThreadPool.h"

    ThreadPool(int size){
    	poolSize = size;
    	assert(sem_init(&mutex, 0, 1)==0);
    	assert(sem_init(&tasksSemaphore, 0, 0)==0);//inicializado en cero, no hay tareas en la cola
		threads[poolSize];
		for (int i = 0; i < size){
			assert(pthread_create( &threads[i], NULL, worker, NULL) == 0);
		}
    }

    ~ThreadPool(){
			for (int i = 0; i < size){
				pthread_cancel( &threads[i], NULL, worker, NULL);
			}
    		sem_destroy(&mutex);
			sem_destroy(&tasksSemaphore);
    }

    void addTask(void *(*start_routine) (void *)){
		sem_wait(&mutex);
			nextTasks.push(start_routine);
		sem_post(&mutex);//libero el mutex
		sem_post(&tasksSemaphore);//Aviso que hay una tarea
    }

    void *worker( void *ptr )
	{
		void* routine = 0;
		while(true){//TODO add way to finish task
			sem_wait(&tasksSemaphore); //espero a que halla alguna tarea
			sem_wait(&mutex);//obtengo el mutex de la cola
				routine = nextTasks.front();
				nextTasks.pop();
			sem_post(&mutex);

			routine(NULL);//llamo a la rutina con parametro null. TODO agregar posiblidad de usar parametros
		}
	}
