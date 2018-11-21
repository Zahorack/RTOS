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
#include <stdint.h>

#include "xsemaphore.h"
#include "xspace.h"
#include "xprocess.h"
#include "xsocket.h"
#include "xtimer.h"
#include "xpacket.h"

//::Function declarations
static void emptyFcn();
static void clientSocketCommunication(socketArgs_t *args);
static void initClientSocket(socketArgs_t *args);
static void socketProcessFcn(Point);
static void sendLidarData();

//::Global variables
socketArgs_t client;

//::Main
int main(int argc, char *argv[]){

	Point rover;

	//INITIALIZATION...
	initSpace(argc,argv);
	initClientSocket(&client);
//	newProcess(socketProcessFcn,emptyFcn,(void *)&rover, NULL);
	socketProcessFcn(rover);
	//CYCLING...
	clientSocketCommunication(&client);

	//DEINITIALIZATION...

	return 0;
}


//::Function definitions
static void socketProcessFcn(Point point){
	printf("I am child process\n");

	const int SendingInterval = 1;
	//send lidar data periodicaly
	initTimer(sendLidarData, SIGUSR1, SendingInterval);

	point.x = LIDAR_RANGE;
	point.y = LIDAR_RANGE;

	while(isAvailable(point) && isFree(point)){
		sleep(1);
		point.x++;
		point.y++;
		updateLidarData(point);
	}
}

static void emptyFcn(){};

static void sendLidarData()
{
	uint16_t dataSize = MAP_SIZE * MAP_SIZE;
	Data_Packet p;
	p.header.mark = PACKET_MARK;
	p.header.data_len = dataSize;
	p.header.type = packet_type_lidar_data;

	p.data = LidarData;
	p.crc = calc_crc8((uint8_t *)&LidarData, dataSize);
/*
			printf("type: %d\n", p.header.type);
                        printf("data_len: %d\n", p.header.data_len);
                        printf("crc: %d\n", p.crc);
*/
	int sendhead = sendSocket(&client.initSocket_fd, (char *)&p, sizeof(Data_Packet) - 1);
	int senddata = sendSocket(&client.initSocket_fd, (char *)p.data, dataSize);

/*	printf("senthead = %d\n", sendhead);
	printf("sentdata = %d\n", senddata);
*/
}

static void sendSimplePacket(uint8_t packet_type)
{
        uint16_t packet_size = sizeof(Data_Packet);                     // - pointer na data
        Data_Packet  p;

        p.header.mark          = PACKET_MARK;
        p.header.data_len      = 0;
        p.header.type          = packet_type;
        p.crc                  = 0;

	sendSocket(&client.initSocket_fd, (char *)&p, packet_size);

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

