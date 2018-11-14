#ifndef XSOCKET_H_
#define XSOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct{

	int recv_fd;
	int send_fd;
	int temp_fd;
} socketArgs_t;


void initServerSocket(socketArgs_t *args);
void initClientSocket(socketArgs_t *args);
void serverSocketCommunication(socketArgs_t *args);
void clientSocketCommunication(socketArgs_t *args);


#endif
