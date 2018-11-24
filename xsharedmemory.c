#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include "xsharedmemory.h"


void initSharedMemory(char *ref, size_t size, key_t key)
{

    int shmid;

     /*
     * Create the segment.
     */
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((ref = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

}


void getSharedMemory(char *ref, size_t size, key_t key)
{

	int shmid;
    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, size, 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((ref = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
}

