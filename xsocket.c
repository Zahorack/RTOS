#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "xsocket.h"

#pragma message "kompilujem xsocket.c ..."

int createSocket(){
   	// vytvorenie socketu
    	int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    	if (sock_desc == -1)
    	{
        	printf("cannot create socket!\n");
        	exit(EXIT_FAILURE);
    	}
	return sock_desc;	//vrati file describtor
}

void checkForSocket(int *fd, struct sockaddr_in *user){
	if (bind(*fd, (struct sockaddr*)user, sizeof(*user)) != 0)
    	{
        	printf("cannot bind socket!\n");
        	close(*fd);
        	exit(EXIT_FAILURE);
    	}

    	if (listen(*fd, 20) != 0)
    	{
        	printf("cannot listen on socket!\n");
        	close(*fd);
        	exit(EXIT_FAILURE);
    	}
}

void connectSocket(int *fd, struct sockaddr_in *user){
	// pripojenie socketu
    	if (connect(*fd, (struct sockaddr*)user, sizeof(*user)) != 0)
    	{
        	printf("cannot connect to server!\n");
        	close(*fd);
		exit(EXIT_FAILURE);
    	}
}

int receiveSocket(socketArgs_t *args, char *data, int length){

	int recieved = recv(args->recv_fd, data, length, 0);
        if (recieved == -1)
        {
            printf("\ncannot read from client!\n");
            exit(EXIT_FAILURE);
        }

        if (recieved == 0)
        {
            printf("\nclient disconnected.\n");
            exit(EXIT_FAILURE);
        }
	return recieved;
}

int sendSocket(socketArgs_t *args, char *data, int length){

	int to_send = length;
	int k = 0;
	int sent =0;

	while (to_send > 0)
        {
            k = send(args->send_fd, data, length, 0);
            if (k == -1)
            {
                printf("cannot write to server!\n");
                break;
            }

            data += k;
            to_send -= k;
	    sent += k;
        }
	return sent;
}
