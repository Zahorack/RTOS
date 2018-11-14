#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "xsocket.h"

#pragma message "kompilujem xsocket.c ..."

//simulacia komunikacie medzi PC a ROVEROM
//budu sa posielat data z lidaru

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

void initServerSocket(socketArgs_t *args)
{
    	struct sockaddr_in server;
	struct sockaddr_in client;
	struct sockaddr_in temp;

	args->temp_fd = createSocket();

	//nastavenie socketu
	memset(&server, 0, sizeof(server));
    	server.sin_family = AF_INET;
    	server.sin_addr.s_addr = INADDR_ANY;
    	server.sin_port = htons(7777);

	checkForSocket(&args->temp_fd, &server);


	memset(&client, 0, sizeof(client));  
    	socklen_t len = sizeof(client);

	args->recv_fd = accept(args->temp_fd, (struct sockaddr*)&client, &len);  
    	if (args->recv_fd == -1)
    	{
        	printf("cannot accept client!\n");
        	close(args->temp_fd);
        	exit(EXIT_FAILURE);
    	}

    	// nastavenie socketu
    	memset(&temp, 0, sizeof(temp));
    	temp.sin_family = AF_INET;
    	temp.sin_addr.s_addr = inet_addr("127.0.0.1");  
    	temp.sin_port = htons(17777);  

	args->send_fd = createSocket();
    	connectSocket(&args->send_fd, &temp);
}

void serverSocketCommunication(socketArgs_t *args)
{
	//demo chat communication
    	char buf[100];
    	char c = '\n';
    	char *p_buf;

	int recs =0;

	while(1){

        	recs = receiveSocket(args, buf, 100);

		if(recs > 0) printf("%*.*s", recs, recs, buf);
        	if (strcmp(buf, "exit") == 0)
            		break;

		gets(buf);

        	int len = strlen(buf);
        	p_buf = buf;

		sendSocket(args, p_buf, len);
		sendSocket(args, &c, 1);

        	if(strcmp(buf, "exit") == 0)
            		break;

    	}

    close(args->recv_fd);
    close(args->temp_fd);
    close(args->send_fd);
    printf("server disconnected\n");

}



void initClientSocket(socketArgs_t *args)
{

    	struct sockaddr_in server;
	struct sockaddr_in client;
	struct sockaddr_in temp;

	args->send_fd = createSocket();

    	// nastavenie socketu
    	memset(&client, 0, sizeof(client));
    	client.sin_family = AF_INET;
    	client.sin_addr.s_addr = inet_addr("127.0.0.1");
    	client.sin_port = htons(7777);

	connectSocket(&args->send_fd, &client);

    	args->temp_fd = createSocket();

	// nastavenie socketu
    	memset(&server, 0, sizeof(server));
    	server.sin_family = AF_INET;
    	server.sin_addr.s_addr = INADDR_ANY;
    	server.sin_port = htons(17777);

	checkForSocket(&args->temp_fd, &server);

    	memset(&temp, 0, sizeof(temp));  
    	socklen_t Second_len = sizeof(temp); 

    	args->recv_fd = accept(args->temp_fd, (struct sockaddr*)&temp, &Second_len);  
    	if (args->recv_fd == -1)
    	{
        	printf("cannot accept client!\n");
        	close(args->temp_fd);
        	exit(EXIT_FAILURE);
    	}
}



void clientSocketCommunication(socketArgs_t *args)
{
    	char buf[100];
    	char c = '\n';
    	char *p_buf;
    	int len = 0;
	int recs = 0;
    	while(1)
    	{
        	gets(buf);

        	len = strlen(buf);
        	p_buf = buf;

		sendSocket(args, p_buf, len);
		sendSocket(args, &c, 1);

        	if (strcmp(buf, "exit") == 0)
            		break;

		recs = receiveSocket(args, buf, 100);

        	if (recs > 0) printf("%*.*s", recs, recs, buf);
        	if (strcmp(buf, "exit") == 0)
	    		break;
    	}

    close(args->recv_fd);
//    close(args->temp_fd);
//    close(args->send_fd);

    printf("client disconnected\n");
}
