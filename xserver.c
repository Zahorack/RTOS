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
static void serverSocketCommunication(socketArgs_t *args);
static void initServerSocket(socketArgs_t *args);

//::Main
int main(int argc, char *argv[]){
	socketArgs_t server;

	//initialization...
	initServerSocket(&server);

	processArgs_t prc = newProcess(childFcn,emptyFcn);

	//cycling...
	serverSocketCommunication(&server);

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

static void serverSocketCommunication(socketArgs_t *args)
{
        //demo chat communication
        char buf[100];
        char c = '\n';
        char *p_buf;

        int recs =0;

        while(1){

                recs = receiveSocket(&args->sharedSocket_fd, buf, 100);

                if(recs > 0) printf("%*.*s", recs, recs, buf);
                if (strcmp(buf, "exit") == 0)
                        break;

                gets(buf);

                int len = strlen(buf);
                p_buf = buf;

                sendSocket(&args->sharedSocket_fd, p_buf, len);
                sendSocket(&args->sharedSocket_fd, &c, 1);

                if(strcmp(buf, "exit") == 0)
                        break;

        }

    close(args->initSocket_fd);
    close(args->sharedSocket_fd);

    printf("Server disconnected!\n");
}

static void initServerSocket(socketArgs_t *args)
{
        struct sockaddr_in server;

        createSocket(args);			//create -> init file describtor

        //socket settings
        memset(&server, 0, sizeof(server));	//clear memory
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;	//look for IP
        server.sin_port = htons(7777);

        checkForSocket(args, &server);		//bind() and listen ()
	acceptSocket(args, &server);
}

