#include "RWLock.h"

#define DEBUG false

RWLock :: RWLock() {
	if (DEBUG == true) printf("Iniciando RWLock\n");
	this->readers = 0;
	assert(sem_init(&mutex, 0, 1)==0);
	this->writing = false;
	if (DEBUG == true) printf("Iniciado RWLock\n");
}

RWLock::~RWLock() {
	sem_destroy(&semaphore);
	while(!lockQueue.empty())
	{
		sem_destroy(lockQueue.front().second);
		lockQueue.pop();		
	}
}

void RWLock :: rlock() {	
	if (DEBUG == true) printf("Reader Lock\n");
	sem_wait(&mutex);
	
	if (!this->lockQueue.empty())
	{		
		assert((!this->writing) || (this->readers == 0));
		
		sem_t semaphore;
		sem_init(&semaphore, 0, 0);
		pair<int,sem_t*> par = make_pair(LECTOR, &semaphore);
		this->lockQueue.push(par);

		sem_post(&mutex);//libero el mutex
		
		sem_wait(&semaphore);//espero mi turno, el contador de lectores se aumenta en callNext()
		sem_destroy(&semaphore);
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers == 1);
			sem_post(&mutex);//libero el mutex
		}
		else if ( this->readers == 0 && this->writing )
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0);
			pair<int,sem_t*> par = make_pair(LECTOR, &semaphore);
			this->lockQueue.push(par);

			sem_post(&mutex);//libero el mutex
		
			sem_wait(&semaphore);//espero mi turno, el contador de lectores se aumenta en callNext()
			sem_destroy(&semaphore);
		}
		else if (this->readers != 0 && !this->writing)
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers > 1);
			sem_post(&mutex);//libero el mutex
		}
		else
		{
			// si llego aca es por que hay lectores y escritores es imposible
			if (DEBUG == true) printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
}

void RWLock :: wlock() {
	if (DEBUG == true) printf("Writer Lock\n");
	sem_wait(&mutex);

	if (!this->lockQueue.empty()){
		assert((!this->writing) || (this->readers == 0));
		
		sem_t semaphore;
		sem_init(&semaphore, 0, 0);
		pair<int,sem_t*> par = make_pair(ESCRITOR, &semaphore);
		this->lockQueue.push(par);

		sem_post(&mutex);//libero el mutex
		if (DEBUG == true) printf("Escritor: espero en mi mutex\n");
		sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
		if (DEBUG == true) printf("Escritor: me despertaron, ya puedo escribir\n");
		sem_destroy(&semaphore);
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->writing = true;
			sem_post(&mutex);//libero el mutex

		}
		else if ( this->readers == 0 && this->writing )
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0);
			pair<int,sem_t*> par = make_pair(ESCRITOR, &semaphore);
			this->lockQueue.push(par);

			sem_post(&mutex);//libero el mutex
			if (DEBUG == true) printf("Escritor: espero en mi mutex\n");
			sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
			if (DEBUG == true) printf("Escritor: me despertaron, ya puedo escribir\n");
			sem_destroy(&semaphore);
		}
		else if (this->readers != 0 && !this->writing)
		{
			sem_t semaphore;
			sem_init(&semaphore, 0, 0);
			pair<int,sem_t*> par = make_pair(ESCRITOR, &semaphore);
			this->lockQueue.push(par);

			sem_post(&mutex);//libero el mutex
			if (DEBUG == true) printf("Escritor: espero en mi mutex\n");
			sem_wait(&semaphore);//espero mi turno, el booleano writing se modifica en callNext()
			if (DEBUG == true) printf("Escritor: me despertaron, ya puedo escribir\n");
			sem_destroy(&semaphore);

		}
		else
		{
			// si llego aca es por que hay lectores y escritores es imposible
			if (DEBUG == true) printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
}

void RWLock :: runlock() {
	if (DEBUG == true) printf("Reader Unlock\n");
	sem_wait(&mutex);

	assert(this->readers > 0 && !this->writing);
	//if ( this->readers != 0 )	// comprobacion por si llegan muchos runlock ESTO NO PUEDE PASAR, ESTA ADENTRO DEL MUTEX
	this->readers--;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			if (DEBUG == true) printf("Soy el último lector, llamo al próximo en la lista\n");
			callNext();
			sem_post(&mutex);//libero el mutex

		}
		else if (this->readers == 0 && this->writing)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			if (DEBUG == true) printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->readers != 0 && !this->writing)
		{
	
			//Salgo, el ultimo lector va a llamar al siguiente
			if (DEBUG == true) printf("Lector: quedan %d lectores y encolados\n", this->readers);
			sem_post(&mutex);//libero el mutex
			
		}
		else
		{
			// si llego aca es por que habia un lector y un escritor corriendo. es imposible
			if (DEBUG == true) printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		assert(this->readers >= 0);
		if (DEBUG == true) printf("Lector: quedan %d lectores\n", this->readers);
		sem_post(&mutex);//libero el mutex
	}
}

void RWLock :: wunlock() {
	if (DEBUG == true) printf("Writer Unlock\n");
	sem_wait(&mutex);
	
	assert(this->writing && this->readers == 0);
	//if ( this->writing )
	this->writing = false;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			if (DEBUG == true) printf("Escritor: llamo al próximo en la lista\n");
			callNext();
			sem_post(&mutex);//libero el mutex

		}
		else
		{
			// si llego aca es por que habia un lector y un escritor corriendo. es imposible
			if (DEBUG == true) printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		//assert(this->readers == 0);
		if (DEBUG == true) printf("Escritor: no hay nadie más\n");
		sem_post(&mutex);//libero el mutex
	}
}	

void RWLock :: callNext(){
	
	pair<int,sem_t*> par = this->lockQueue.front();
	
	if(par.first == LECTOR){
		if (DEBUG == true) printf("Es Lector\n");			
		while(!lockQueue.empty()){
			par = this->lockQueue.front();
			if(par.first == ESCRITOR) break;
			this->lockQueue.pop();
			this->readers++;
			sem_post(par.second);
		}
	}
	else{
		if (DEBUG == true) printf("Es Escritor\n");			
		this->lockQueue.pop();
		this->writing = true;
		sem_post(par.second);
	}
	
}

/*tipo de datos pthread mutex t
crear mutex pthread mutex init(mutex, attr)
destruir mutex pthread mutex destroy(mutex)
espera bloqueante pthread mutex lock(mutex)
intento no bloqueante pthread mutex trylock(mutex)
liberaci ́n (signal) pthread mutex unlock(mutex)
*/
