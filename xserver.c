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
#include "xsharedmemory.h"

//::Function declarations
static void emptyFcn();
static void initServerSocket(socketArgs_t *args);
static void parseLidarData(char *, uint16_t);
static void *receiveFromClient();
static void getLidarData();
static void sendSimplePacket(uint8_t );
static void sendMoveCommand(uint8_t move);
static void getMoveCommand();
static void waitForClientInit();
static void clientReady();
static void initScreen();
static void sendProcessID();
static void updateLidarScreen();
static void setClientPID(uint8_t *);
static void stopHandler();

//::Blobal variables
socketArgs_t server;
uint8_t clientFlag = 0;
pid_t clientPID;
WINDOW *lidar =NULL;

//::Main
int main(int argc, char *argv[]){
	pthread_t thread1;
	char *sharedTask;

	//initialization...
	signal(SIGINT, stopHandler);
	initServerSocket(&server);
	initSharedMemory(sharedTask, 32, 5678);
	createThread(&thread1, receiveFromClient, &server);
	waitForClientInit();

	initScreen();
        initTimer(getLidarData, SIGUSR1, 1, 0);
//	newProcess(receiveFromClient,emptyFcn, (void*)&server, NULL);

	//cycling...
	getMoveCommand();

	joinThread(&thread1);
	endwin();

	return 0;
}

//::Function definitions
static void emptyFcn(){};

static void stopHandler()
{

	close(server.initSocket_fd);
        close(server.sharedSocket_fd);

	endwin();
	printf("User wish exit progam!\n");
	kill(clientPID, SIGINT);

	kill(getpid(), SIGQUIT);	//SIGQUIT close socket port
}

static void waitForClientInit()
{
	signal(SIGUSR2, clientReady);
	sendProcessID();

	while(!clientFlag) {
		sleep(1);
		sendProcessID();
		printf("Wait for client initialization...\n");
		printf("pid: %d\n", getpid());
	}

}

static void clientReady()
{
	clientFlag = 1;
}

static void initScreen()
{
	initscr();
	lidar = newwin(MAP_SIZE+2, MAP_SIZE+2, 2,2);

	printw("Lidar view");

	box(lidar,0,0);
	cbreak();
	noecho();
	wrefresh(lidar);
	refresh();
}

static void updateLidarScreen()
{
        int iterator = 0;
        int ln = MAP_SIZE;
        for(int i =0; i < ln; i++){
                for(int j = 0; j < ln; j++){
                        mvwaddch(lidar, i+1,j+1,LidarData[iterator]);
                        iterator++;
                }
                waddch(lidar,'\n');
        }
	box(lidar,0,0);
	wrefresh(lidar);
}

static void *receiveFromClient(socketArgs_t *args)
{
	uint16_t word;
	Data_Packet p;
	int recs =0;

	while(1) {
		recs = receiveSocket(&args->sharedSocket_fd, (char*)&word, 2);

		if(word == PACKET_MARK) {
			//Readpacket header and CRC
			recs = receiveSocket(&args->sharedSocket_fd, (char*)&p+2, sizeof(Data_Packet) - 3);
/*			printf("header type: %d\n", p.header.type);
			printf("crc : %d\n", p.crc);
			printf("datlen : %d\n", p.header.data_len);
*/
			if(p.header.data_len > 0) {
				//Dangerous- Dynamic allocaton!!!!
				p.data = malloc(p.header.data_len * sizeof(uint8_t));
				if(p.data == NULL) printf("ERROR ALLOCATION\n");
				//Read data
				receiveSocket(&args->sharedSocket_fd, (char*)p.data, p.header.data_len);
				//Calc CRC
				uint8_t calcCRC = calc_crc8((uint8_t *)p.data, p.header.data_len);
//				printf("calcCRC : %d\n", calcCRC);
				if(p.crc == calcCRC)
					//PARSING PACKETS
					switch(p.header.type) {
						case packet_type_lidar_data: parseLidarData((char*)p.data, p.header.data_len); break;
						case packet_type_pid : setClientPID(p.data);
					}
				free(p.data);	//Uvolinit pamat
			}
			else {
				switch(p.header.type) {

				}
			}
		} //PACKET MARK NOT FOOUND
	} //REPEAT

    	close(args->initSocket_fd);
    	close(args->sharedSocket_fd);

    	printf("Server disconnected!\n");
}

static void setClientPID(uint8_t *data)
{
        uint16_t pID = data[0] | data[1] << 8;
        clientPID = pID;
}

static void sendSimplePacket(uint8_t packet_type)
{
        uint16_t packet_size = sizeof(Data_Packet);
        Data_Packet  p;

        p.header.mark          = PACKET_MARK;
        p.header.data_len      = 0;
        p.header.type          = packet_type;
        p.crc                  = 0;
	p.data		       = NULL;
        sendSocket(&server.sharedSocket_fd, (char*)&p, packet_size -1);
}

static void sendMoveCommand(uint8_t move)
{
	uint16_t packet_size = sizeof(Data_Packet);
        Data_Packet  p;
	uint8_t *ptr = &move;

        p.header.mark          = PACKET_MARK;
        p.header.data_len      = 1;
        p.header.type          = packet_type_move;
        p.crc                  = calc_crc8((uint8_t *)ptr, 1);
        p.data                 = ptr;
	sendSocket(&server.sharedSocket_fd, (char *)&p, packet_size -1);
	sendSocket(&server.sharedSocket_fd, (char *)p.data, 1);

}

static void sendProcessID()
{
	uint16_t packet_size = sizeof(Data_Packet);
        Data_Packet  p;
	pid_t processID= getpid();
	uint8_t *ptr = (uint8_t *)&processID;

        p.header.mark          = PACKET_MARK;
        p.header.data_len      = sizeof(pid_t);
        p.header.type          = packet_type_pid;
        p.crc                  = calc_crc8((uint8_t *)ptr, p.header.data_len);
        p.data                 = ptr;
	sendSocket(&server.sharedSocket_fd, (char *)&p, packet_size -1);
	sendSocket(&server.sharedSocket_fd, (char *)p.data, sizeof(pid_t));
}

static void getMoveCommand()
{
	char c;
	while(1) {
//		printf("%s", c);
		switch(getch()) {
			case 'w': sendMoveCommand(up); break;
			case 's': sendMoveCommand(down); break;
			case 'd': sendMoveCommand(right); break;
			case 'a': sendMoveCommand(left); break;

			case '\n' : break;
			default : printf("Invalit command!\n"); break;
		}
	getLidarData();
	}
}

static void getLidarData()
{
	sendSimplePacket(packet_type_lidar_data);
}

static void parseLidarData(char * data, uint16_t length)
{
	int len = MAP_SIZE * MAP_SIZE;

                strncpy(LidarData, data, length);
		updateLidarScreen();


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

