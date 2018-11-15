/* FEI STU, predmet RTOS
 * Semestralne zadanie
 *
 * Autor: Oliver Holly
 * Date:  11.11.2018
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "xsemaphore.h"
#include "main.h"
#include "xspace.h"
#include "xprocess.h"
#include "xsocket.h"

//::Function declarations
static void childFcn();
static void parentFcn();
static void emptyFcn();
static void clientSocketCommunication(socketArgs_t *args);
static void initClientSocket(socketArgs_t *args);
static void socketProcessFcn();

//::Main
int main(int argc, char *argv[]){
	socketArgs_t args;
	Point rover;
	rover.x = 0;
	rover.y = 0;

	//INITIALIZATION
	initSpace(argc,argv);
//	initClientSocket(&args);
//	processArgs_t socketProces = newProcess(socketProcessFcn,emptyFcn);

	//CYCLING...
	while(1){
		sleep(1);
		rover.x++;
		rover.y++;
		updateMap(rover);
	}
//	clientSocketCommunication(&args);

	//DEINITIALIZATION...


	return 0;
}


//::Function definitions
static void childFcn(){
	for(int i=0; i < 1; i++){
		printf("I am child process\n");
	}
}

static void parentFcn(){
	for(int i = 0; i < 1; i++){
		printf("I am parrent process\n");
	}
}

static void emptyFcn(){};

static void socketProcessFcn(){
	//create struct with file describtors
	socketArgs_t client;
	//init communiication setting
	initClientSocket(&client);
	//start communication..
	clientSocketCommunication(&client);
}

static void clientSocketCommunication(socketArgs_t *args)
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

static void initClientSocket(socketArgs_t *args)
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

        args->recv_fd = accept(args->temp_fd, (struct sockaddr*)&temp,&Second_len);
        if (args->recv_fd == -1)
        {
                printf("cannot accept client!\n");
                close(args->temp_fd);
                exit(EXIT_FAILURE);
        }
}

