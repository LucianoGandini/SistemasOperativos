#ifndef RWLock_h
#define RWLock_h
#include <iostream>
#include <queue>
#include <pair>
#include <pthread.h>

#define LECTOR 1
#define ESCRITOR 0



class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
    	void callNext();
		pthread_mutex_t mutex;
		unsigned int readers;
		bool writing;
		queue<pair<int,pthread_mutex_t>> lockQueue;
};

#endif
