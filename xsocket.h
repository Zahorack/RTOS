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

	int initSocket_fd;
	int sharedSocket_fd;
} socketArgs_t;

void createSocket(socketArgs_t *);
void checkForSocket(socketArgs_t *, struct sockaddr_in *);
void connectSocket(socketArgs_t *, struct sockaddr_in *);
void acceptSocket(socketArgs_t *, struct sockaddr_in *);
int  receiveSocket(int *fd, char *, int);
int  sendSocket(int *fd, char *, int);

#endif
