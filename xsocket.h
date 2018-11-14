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

int createSocket();
void checkForSocket(int *, struct sockaddr_in *);
void connectSocket(int *, struct sockaddr_in *);
int receiveSocket(socketArgs_t *args, char *, int);
int sendSocket(socketArgs_t *, char *, int);

#endif
