#ifndef Thread_Pool
#define Thread_Pool
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <utility>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>
#include <vector>

using namespace std;

class ThreadPool {
    public:
        ThreadPool(int size);
        ~ThreadPool();
        void addTask(void *(*start_routine) (void *));

    private:
    	void *worker( void *ptr );

    	int poolSize;

    	void callNext();
		
		sem_t mutex, tasksSemaphore;
				
		pthread_t threads[];
		
		std::queue<void *(*start_routine) (void *)> nextTasks;