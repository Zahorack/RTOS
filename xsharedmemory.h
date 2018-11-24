#ifndef XSHAREDMEMORY_H_
#define XSHAREDMEMORY_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void initSharedMemory(char *ref, size_t size, key_t key);
void getSharedMemory(char *ref, size_t size, key_t key);


#endif

