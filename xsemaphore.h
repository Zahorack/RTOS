#ifndef XSEMAPHORE_H_
#define XSEMAPHORE_H_

// Struktura so zdielanymi premennymi
typedef struct {
        sem_t sem;
        int number;
} threadArgs_t;

// Kedze sme v C mozeme dat pointer na strukturu rovno do argumentu
void *threadFunction1(threadArgs_t *args);
void *threadFunction2(threadArgs_t *args);

void createThread(pthread_t *thread, void *(*threadFunction)(threadArgs_t *), threadArgs_t *args);
void joinThread(pthread_t *thread);
int  initSemaphore();
int  deinitSemaphore();


#endif
