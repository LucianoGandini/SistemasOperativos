#include "RWLock.h"

RWLock :: RWLock() {
	this->readers = 0;
	//pthread_mutex_init(&mutex, NULL); 
	this->writing = false;
}

void RWLock :: rlock() {	

	pthread_mutex_lock(&mutex_readers);
	pthread_mutex_lock(&mutex_writing);
	pthread_mutex_lock(&mutex_lockQueue);
	
	if (!this->lockQueue.empty())
	{
		
		assert((!this->writing) || (this->readers == 0));
		
		pthread_mutex_t readMutex;
		pthread_mutex_init(&readMutex, NULL); 
		pair<int,pthread_mutex_t> par = make_pair(LECTOR, readMutex);
		this->lockQueue.push(par);

		pthread_mutex_lock(&readMutex);//inicializar en 0 el mutex

		pthread_mutex_unlock(&mutex_lockQueue);	
		pthread_mutex_unlock(&mutex_writing);
		pthread_mutex_unlock(&mutex_readers);
		
		pthread_mutex_lock(&readMutex);//esto lo hace		
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers >= 1);
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

		}
		else if ( this->readers == 0 && this->writing )
		{
			pthread_mutex_t readMutex;
			pthread_mutex_init(&readMutex, NULL); 
			pair<int,pthread_mutex_t> par = make_pair(LECTOR, readMutex);
			this->lockQueue.push(par);

			pthread_mutex_lock(&readMutex);//inicializar en 0 el mutex

			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);
		
			pthread_mutex_lock(&readMutex);//esto lo hace

		}
		else if (this->readers != 0 && !this->writing)
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers >= 1);
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

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
	pthread_mutex_lock(&mutex_readers);
	pthread_mutex_lock(&mutex_writing);
	pthread_mutex_lock(&mutex_lockQueue);


	if (!this->lockQueue.empty()){
		assert((!this->writing) || (this->readers == 0));
		
		pthread_mutex_t readMutex;
		pthread_mutex_init(&readMutex, NULL); 
		pair<int,pthread_mutex_t> par = make_pair(ESCRITOR, readMutex);
		this->lockQueue.push(par);

		pthread_mutex_lock(&readMutex);//inicializar en 0 el mutex

		pthread_mutex_unlock(&mutex_lockQueue);	
		pthread_mutex_unlock(&mutex_writing);
		pthread_mutex_unlock(&mutex_readers);
		
		pthread_mutex_lock(&readMutex);//esto lo hace		
	}
	else
	{
		if ( this->readers == 0 && !this->writing )
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers >= 1);
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

		}
		else if ( this->readers == 0 && this->writing )
		{
			pthread_mutex_t readMutex;
			pthread_mutex_init(&readMutex, NULL); 
			pair<int,pthread_mutex_t> par = make_pair(ESCRITOR, readMutex);
			this->lockQueue.push(par);

			pthread_mutex_lock(&readMutex);//inicializar en 0 el mutex

			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);
		
			pthread_mutex_lock(&readMutex);//esto lo hace

		}
		else if (this->readers != 0 && !this->writing)
		{
			assert(this->lockQueue.empty());
			this->readers++;
			assert(this->readers >= 1);
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

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
	pthread_mutex_lock(&mutex_readers);
	pthread_mutex_lock(&mutex_writing);
	pthread_mutex_lock(&mutex_lockQueue);

	assert(this->readers > 0);
	if ( this->readers != 0 )	// comprobacion por si llegan muchos runlock 
		this->readers--;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			callNext();
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

		}
		else if (this->readers == 0 && this->writing)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->readers != 0 && !this->writing)
		{
	
			//Llamo solo a los primeros de la coloa que sean lectores
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);
			
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
		pthread_mutex_unlock(&mutex_lockQueue);	
		pthread_mutex_unlock(&mutex_writing);
		pthread_mutex_unlock(&mutex_readers);
	}
}

void RWLock :: wunlock() {
	pthread_mutex_lock(&mutex_readers);
	pthread_mutex_lock(&mutex_writing);
	pthread_mutex_lock(&mutex_lockQueue);
	
	assert(this->writing == true);
	if ( this->writing )
		this->writing = false;
	
	if (!this->lockQueue.empty())
	{

		if ( this->readers == 0 && !this->writing )
		{
			callNext();
			pthread_mutex_unlock(&mutex_lockQueue);	
			pthread_mutex_unlock(&mutex_writing);
			pthread_mutex_unlock(&mutex_readers);

		}
		else if (this->readers == 0 && this->writing)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->readers != 0 && !this->writing)
		{
			// si llego aca es por que hay 2 o mas lectores corriendo mientras estaba corriendo mi escritor. es imposible
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
		assert(this->readers == 0);
		pthread_mutex_unlock(&mutex_lockQueue);	
		pthread_mutex_unlock(&mutex_writing);
		pthread_mutex_unlock(&mutex_readers);
	}
}	

void RWLock :: callNext(){
	
	pair<int,pthread_mutex_t> par = this->lockQueue.front();

	
	if(par.first == LECTOR){
		printf("Es Lector\n");			
		while(!lockQueue.empty()){
			par = this->lockQueue.front();
			if(par.first == ESCRITOR) break;
			this->lockQueue.pop();
			this->readers++;
			pthread_mutex_unlock(&par.second);
		}
	}
	else{
		printf("Es Escritor\n");			
		this->lockQueue.pop();
		this->writing = true;
		pthread_mutex_unlock(&par.second);
	}
	
}

/*tipo de datos pthread mutex t
crear mutex pthread mutex init(mutex, attr)
destruir mutex pthread mutex destroy(&mutex)
espera bloqueante pthread mutex lock(&mutex)
intento no bloqueante pthread mutex trylock(&mutex)
liberaci ́n (signal) pthread mutex unlock(&mutex)
*/
