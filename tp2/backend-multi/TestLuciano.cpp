#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "RWLock.h"

#define THREADS 20

void *nothing_function( void *ptr );
void *reader_function( void *ptr );
void *writer_function( void *ptr );
int resource = 333;
RWLock lock;

main()
{
	lock = RWLock();
	pthread_t thread[THREADS];
	int firstCompare = 666;
	int secondCompare = 123456789;
	int ret[THREADS];

	int i = 0;
	/* Create independent threads each of which will execute function */
	
	ret[i] = pthread_create( &thread[i], NULL, writer_function, (void*) firstCompare);
	i++;
	ret[i] = pthread_create( &thread[i], NULL, nothing_function, (void*) firstCompare);
	i++;
	pthread_join( thread[1], NULL);
	while(i < THREADS/2){
		ret[i] = pthread_create( &thread[i], NULL, reader_function, (void*) firstCompare);
		i++;
	}
	
	pthread_join( thread[THREADS/2], NULL); //Espero que termine el último que lance, no necesariamente terminaron los otros.
	ret[i] = pthread_create( &thread[i], NULL, writer_function, (void*) secondCompare);
	i++;
	
	while(i < THREADS){
		ret[i] = pthread_create( &thread[i], NULL, reader_function, (void*) secondCompare);
		i++;
	}
	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */

	for( i = 0; i < THREADS; i++){
		pthread_join( thread[i], NULL);
		printf("Thread %d returns: %d\n", i , ret[i]);
	}

	exit(0);
}
void *nothing_function( void *ptr )
{
	int number;
	number = (long) ptr;
	printf("Im a Nothing with number: %d\n", number);
	return 0;
}

void *writer_function( void *ptr )
{
	int number;
	number = (long) ptr;
	lock.wlock();
	printf("Im a Writer with number: %d\n", number);
	resource = number;
	printf("Wrote: %d\n", resource);
	lock.wunlock();
	return 0;
}

void *reader_function( void *ptr )
{
	int number;
	number = (long) ptr;
	lock.rlock();
	printf("Im a Reader with number: %d\n", number);
	assert (resource == number);
	printf("Readed: %d\n", resource);
	lock.runlock();
	return 0;
}

