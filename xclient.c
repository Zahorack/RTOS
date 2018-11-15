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

	//INITIALIZATION...
	initSpace(argc,argv);
	initClientSocket(&args);
	processArgs_t socketProces = newProcess(childFcn,emptyFcn);

	//CYCLING...
	clientSocketCommunication(&args);

	//DEINITIALIZATION...


	return 0;
}


//::Function definitions
static void childFcn(){
	printf("I am child process\n");

	Point rover;
	rover.x = 0;
	rover.y = 0;

	while(isAvailable(rover)){
		sleep(1);
		rover.x++;
		rover.y++;
		updateMap(rover);
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

                sendSocket(&args->initSocket_fd, p_buf, len);
                sendSocket(&args->initSocket_fd, &c, 1);

                if (strcmp(buf, "exit") == 0)
                        break;

                recs = receiveSocket(&args->initSocket_fd, buf, 100);

                if (recs > 0) printf("%*.*s", recs, recs, buf);
                if (strcmp(buf, "exit") == 0)
			break;
       }

    close(args->sharedSocket_fd);
    close(args->initSocket_fd);

    printf("client disconnected\n");
}

static void initClientSocket(socketArgs_t *args)
{
        struct sockaddr_in client;

        createSocket(args);

        // nastavenie socketu
        memset(&client, 0, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_addr.s_addr = inet_addr("127.0.0.1");
        client.sin_port = htons(7777);

        connectSocket(args, &client);
}

