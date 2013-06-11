#include "RWLock.h"

RWLock :: RWLock() {
	readers = 0;
	pthread_mutex_init(mutex, NULL); 
	writing = false;
}

void RWLock :: rlock() {
	pthread_mutex_lock(mutex);
	if (writting || !lockQueue.empty()){
		pthread_mutex_t readMutex;
		pthread_mutex_init(readMutex, NULL); 
		pair<bool,pthread_mutex_t> par = new pair<bool,pthread_mutex_t>(LECTOR, readMutex);
		lockQueue.push(par);

		pthread_mutex_unlock(mutex);
		pthread_mutex_lock(readMutex);//inicializar en 0 el mutex
		pthread_mutex_lock(readMutex);//esto lo hace
		pthread_mutex_destroy(readMutex);
		pthread_mutex_lock(mutex);
	}
	readers++;
	pthread_mutex_unlock(mutex);
}

void RWLock :: wlock() {
	pthread_mutex_lock(mutex);
	if (writting || !lockQueue.empty() || readers != 0){
		pthread_mutex_t writeMutex;
		pthread_mutex_init(writeMutex, NULL);
		pair<bool,pthread_mutex_t> par = pair(ESCRITOR, writeMutex);
		lockQueue.push(par);

		pthread_mutex_unlock(mutex);
		pthread_mutex_lock(writeMutex);//inicializar en 0 el mutex
		pthread_mutex_lock(writeMutex);//esto lo hace
		pthread_mutex_destroy(writeMutex);
		pthread_mutex_lock(mutex);
	}
	writing = true;
	pthread_mutex_unlock(mutex);
}

void RWLock :: runlock() {
	pthread_mutex_lock(mutex);
	readers--;
	if(readers == 0 && !lockQueue.empty()){
		callNext();
	}
	pthread_mutex_unlock(mutex);
}

void RWLock :: wunlock() {
	pthread_mutex_lock(mutex);
	writing = false;
	if(!lockQueue.empty()){
		callNext();
	}
	pthread_mutex_unlock(mutex);
}

void callNext(){
	pair<bool,pthread_mutex_t> par = lockQueue.front();

	
	if(par.first == LECTOR){
				
		while(!lockQueue.empty()){
			par = lockQueue.front();
			if(par.first == ESCRITOR) break;
			
			lockQueue.pop();
			pthread_mutex_unlock(pair.second);
		}
	}
	else{
		lockQueue.pop();
		pthread_mutex_unlock(pair.second);
	}
	
}

/*tipo de datos pthread mutex t
crear mutex pthread mutex init(mutex, attr)
destruir mutex pthread mutex destroy(mutex)
espera bloqueante pthread mutex lock(mutex)
intento no bloqueante pthread mutex trylock(mutex)
liberaci ́n (signal) pthread mutex unlock(mutex)
*/
