#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "xsemaphore.h"

//#pragma message "kompilujem xsemaphore"

pthread_t thread1, thread2;
threadArgs_t args;


// Kedze vytvarame viac ako 1 vlakno spravime si na to funkciu nech nemame duplicitny kod
void createThread(pthread_t *thread, void *(*threadFunction)(threadArgs_t *), void *args)
{
	// A posleme argumenty!
	if(pthread_create(thread, NULL, (void *)threadFunction, (void *)args)) { 
		perror("Thread creation failed\n");
		exit(EXIT_FAILURE);
	}
}

void joinThread(pthread_t *thread)
{
	void *threadResult; // Zahodime

	if(pthread_join(*thread, &threadResult)) { // != 0 je zbytocne
		perror("Thread join failed\n");
		exit(EXIT_FAILURE);
	}
}

int initSemaphore() {
	// Premenne nemusia byt globalne a dame ich vlaknam cez void *arg

	// Pripravime
	args.number = 0;

	if(sem_init(&args.sem, 0, 0)) { // != 0 je zbytocne
		perror("Semaphore initialization failed");
		exit(EXIT_FAILURE);
	}

	// Vytvorime vlakna
	createThread(&thread1, threadFunction1, &args);
	createThread(&thread2, threadFunction2, &args);

	return EXIT_SUCCESS;
}

int deinitSemaphore() {
	printf("Waiting for thread 1 to finish\n");
	joinThread(&thread1);

	printf("Waiting for thread 2 to finish\n");
	joinThread(&thread2);

	printf("Threads joined\n");

	sem_destroy(&args.sem);

	return EXIT_SUCCESS;
}

void *threadFunction1(threadArgs_t *args)
{
	printf("Thread 1: Started\n");

	pthread_exit(NULL);
}

void *threadFunction2(threadArgs_t *args)
{
	printf("Thread 2: Started\n");

	pthread_exit(NULL);
}

