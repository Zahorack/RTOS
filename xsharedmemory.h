#ifndef XSHAREDMEMORY_H_
#define XSHAREDMEMORY_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

uint8_t *initSharedMemory(size_t size, key_t key);
uint8_t *getSharedMemory(size_t size, key_t key);


#endif

