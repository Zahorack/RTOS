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
#include <ncurses.h>

#include "xsemaphore.h"
#include "xspace.h"
#include "xprocess.h"
#include "xsocket.h"
#include "xtimer.h"
#include "xpacket.h"

//::Function declarations
static void emptyFcn();
static void serverSocketCommunication(socketArgs_t *args);
static void initServerSocket(socketArgs_t *args);
static void readLidarData(socketArgs_t *);
static void receiveFromClient();

//::Main
int main(int argc, char *argv[]){
	socketArgs_t server;

	//initialization...
	initServerSocket(&server);
//	newProcess(receiveFromClient,emptyFcn, (void*)&server, NULL);

	//cycling...
	receiveFromClient(&server);

	return 0;
}

//::Function definitions
static void emptyFcn(){};

static void receiveFromClient(socketArgs_t *args)
{
	uint16_t word;
	Data_Packet p;
	int recs =0;

	while(1) {
		recs = receiveSocket(&args->sharedSocket_fd, (char*)&word, 2);

		if(word == PACKET_MARK) {
			recs = receiveSocket(&args->sharedSocket_fd, (char*)&p+2, sizeof(Data_Packet) - 3);
/*			printf("type: %d\n", p.header.type);
			printf("data_len: %d\n", p.header.data_len);
			printf("crc: %d\n", p.crc);
			printf("recs head lentgh = %d\n", recs);

*/			//PARSING PACKETS
			switch(p.header.type) {
				case packet_type_lidar_data: readLidarData(args); break;
			}
		}
	}
    close(args->initSocket_fd);
    close(args->sharedSocket_fd);

    printf("Server disconnected!\n");
}

static void readLidarData(socketArgs_t *args)
{
	int len = MAP_SIZE * MAP_SIZE;
        char buf[len];
        char *p_buf;
        int recs =0;

                recs = receiveSocket(&args->sharedSocket_fd, LidarData, len);
		printLidarData();

}

static void serverSocketCommunication(socketArgs_t *args)
{
        //demo chat communication
        char buf[100];
        char c = '\n';
        char *p_buf;

        int recs =0;

        while(1){

                recs = receiveSocket(&args->sharedSocket_fd, buf, 100);

                if(recs > 0) printf("%s\n", buf);
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

