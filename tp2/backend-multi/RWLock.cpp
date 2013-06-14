#include "RWLock.h"

RWLock :: RWLock() {
	this->readers = 0;
	pthread_mutex_init(&mutex, NULL); 
	this->writing = false;
}

void RWLock :: rlock() {	

	pthread_mutex_lock(&mutex);
	
	if (!this->lockQueue.empty())
	{		
		assert((!this->writing) || (this->readers == 0));
		
		sem_t semaphore;
		sem_init(&semaphore, 0, 0); 
		pair<int,sem_t> par = make_pair(LECTOR, semaphore);
		this->lockQueue.push(par);

		pthread_mutex_unlock(&mutex);//libero el mutex
		
		sem_wait(&semaphore);//espero mi turno, el contador de lectores se aumenta en callNext()
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers == 1);
			pthread_mutex_unlock(&mutex);
		}
		else if ( this->readers == 0 && this->writing )
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0); 
			pair<int,sem_t> par = make_pair(LECTOR, semaphore);
			this->lockQueue.push(par);

			pthread_mutex_unlock(&mutex);//libero el mutex
		
			sem_wait(&semaphore);//espero mi turno, el contador de lectores se aumenta en callNext()
		}
		else if (this->readers != 0 && !this->writing)
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers > 1);
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			// si llego aca es por que hay lectores y escritores es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
}

void RWLock :: wlock() {
	pthread_mutex_lock(&mutex);

	if (!this->lockQueue.empty()){
		assert((!this->writing) || (this->readers == 0));
		
		sem_t semaphore;
		sem_init(&semaphore, 0, 0); 
		pair<int,sem_t> par = make_pair(ESCRITOR, semaphore);
		this->lockQueue.push(par);
		pthread_mutex_unlock(&mutex);
		printf("Escritor: espero en mi mutex\n");
		sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
		printf("Escritor: me despertaron, ya puedo escribir\n");
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->writing = true;
			pthread_mutex_unlock(&mutex);

		}
		else if ( this->readers == 0 && this->writing )
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0); 
			pair<int,sem_t> par = make_pair(ESCRITOR, semaphore);
			this->lockQueue.push(par);
			pthread_mutex_unlock(&mutex);
			printf("Escritor: espero en mi mutex\n");
			sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
			printf("Escritor: me despertaron, ya puedo escribir\n");
		}
		else if (this->readers != 0 && !this->writing)
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0); 
			pair<int,sem_t> par = make_pair(ESCRITOR, semaphore);
			this->lockQueue.push(par);
			pthread_mutex_unlock(&mutex);
			printf("Escritor: espero en mi mutex\n");
			sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
			printf("Escritor: me despertaron, ya puedo escribir\n");

		}
		else
		{
			// si llego aca es por que hay lectores y escritores es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
}

void RWLock :: runlock() {
	pthread_mutex_lock(&mutex);

	assert(this->readers > 0 && !this->writing);
	//if ( this->readers != 0 )	// comprobacion por si llegan muchos runlock ESTO NO PUEDE PASAR, ESTA ADENTRO DEL MUTEX
	this->readers--;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			printf("Soy el último lector, llamo al próximo en la lista\n");
			callNext();
			pthread_mutex_unlock(&mutex);

		}
		else if (this->readers == 0 && this->writing)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->readers != 0 && !this->writing)
		{
	
			//Salgo, el ultimo lector va a llamar al siguiente
			pthread_mutex_unlock(&mutex);
			
		}
		else
		{
			// si llego aca es por que habia un lector y un escritor corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		assert(this->readers >= 0);
		printf("Soy el último lector, no hay nadie más\n");
		pthread_mutex_unlock(&mutex);
	}
}

void RWLock :: wunlock() {
	pthread_mutex_lock(&mutex);
	
	assert(this->writing && this->readers == 0);
	//if ( this->writing )
	this->writing = false;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			printf("Escritor: llamo al próximo en la lista\n");
			callNext();
			pthread_mutex_unlock(&mutex);

		}
		else if (this->readers == 0 && this->writing)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->readers != 0 && !this->writing)
		{
			// si llego aca es por que hay 1 o mas lectores corriendo mientras estaba corriendo mi escritor. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);	
		}
		else
		{
			// si llego aca es por que habia un lector y un escritor corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		//assert(this->readers == 0);
		printf("Escritor: no hay nadie más\n");
		pthread_mutex_unlock(&mutex);
	}
}	

void RWLock :: callNext(){
	
	pair<int,sem_t> par = this->lockQueue.front();

	
	if(par.first == LECTOR){
		printf("Es Lector\n");			
		while(!lockQueue.empty()){
			par = this->lockQueue.front();
			if(par.first == ESCRITOR) break;
			this->lockQueue.pop();
			this->readers++;
			sem_post(&(par.second));
			//TODO sem_destroy(&(par.second));//undefined behaviour
		}
	}
	else{
		printf("Es Escritor\n");			
		this->lockQueue.pop();
		this->writing = true;
		sem_post(&(par.second));
		//TODO sem_destroy(&(par.second));//undefined behaviour
	}
	
}

/*tipo de datos pthread mutex t
crear mutex pthread mutex init(mutex, attr)
destruir mutex pthread mutex destroy(&mutex)
espera bloqueante pthread mutex lock(&mutex)
intento no bloqueante pthread mutex trylock(&mutex)
liberaci ́n (signal) pthread mutex unlock(&mutex)
*/
