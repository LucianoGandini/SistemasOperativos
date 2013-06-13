#ifndef RWLock_h
#define RWLock_h
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <utility>
#include <pthread.h>

#define LECTOR 1
#define ESCRITOR 0

using namespace std;

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
		std::queue< pair<int,pthread_mutex_t> > lockQueue;
};

#endif
