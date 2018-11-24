#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "xsocket.h"

//#pragma message "kompilujem xsocket.c ..."

void createSocket(socketArgs_t *args){
   	// vytvorenie socketu
    	args->initSocket_fd = socket(AF_INET, SOCK_STREAM, 0);
    	if (args->initSocket_fd == -1)
    	{
        	printf("cannot create socket!\n");
		kill(getpid(), SIGKILL);
        	exit(EXIT_FAILURE);
    	}
}

void checkForSocket(socketArgs_t *args, struct sockaddr_in *user){
	//chceck if port is available
	if (bind(args->initSocket_fd, (struct sockaddr*)user, sizeof(*user)) != 0)
    	{
        	printf("cannot bind socket!\n");
        	close(args->initSocket_fd);
		kill(getpid(), SIGKILL);
        	exit(EXIT_FAILURE);
    	}
	//start parallel process and wait for other 20 clients
    	if (listen(args->initSocket_fd, 20) != 0)
    	{
        	printf("cannot listen on socket!\n");
        	close(args->initSocket_fd);
		kill(getpid(), SIGKILL);
        	exit(EXIT_FAILURE);
    	}
}

void connectSocket(socketArgs_t *args, struct sockaddr_in *user){
	// pripojenie socketu
    	if (connect(args->initSocket_fd, (struct sockaddr*)user, sizeof(*user)) != 0)
    	{
        	printf("cannot connect to server!\n");
        	close(args->initSocket_fd);
		kill(getpid(), SIGKILL);
		exit(EXIT_FAILURE);
    	}
}

void acceptSocket(socketArgs_t *args, struct sockaddr_in *user){

	socklen_t len = sizeof(user);

	args->sharedSocket_fd = accept(args->initSocket_fd, (struct sockaddr*)&user,&len);
        if (args->sharedSocket_fd == -1)
        {
                printf("cannot accept client!\n");
                close(args->sharedSocket_fd);
		kill(getpid(), SIGKILL);
	        exit(EXIT_FAILURE);
        }
}

int receiveSocket(int *fd, char *data, int length){

	int recieved = recv(*fd, data, length, 0);
        if (recieved == -1)
        {
            printf("\ncannot read from client!\n");
	    kill(getpid(), SIGKILL);
	    exit(EXIT_FAILURE);
        }

	return recieved;
}

int sendSocket(int *fd, char *data, int length){

	int to_send = length;
	int k = 0;
	int sent =0;

	while (to_send > 0)
        {
            k = send(*fd, data, length, 0);
            if (k == -1)
            {
                printf("cannot write to server!\n");
		kill(getpid(), SIGKILL);
                break;
            }

            data += k;
            to_send -= k;
	    sent += k;
        }
	return sent;
}
