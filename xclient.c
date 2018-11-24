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
static void initClientSocket(socketArgs_t *args);
static void sendLidarData();
static void *receiveFromServer();
static void initRover();
static void nextMove(int, int);
static void updateRover(uint8_t);
static void initScreen();
static void repaintMap();
static void setServerPID(uint8_t *);
static void stopHandler();
static void sendProcessID();

//::Global variables
socketArgs_t client;
Point rover;
pid_t serverPID;
WINDOW *mapa = NULL;

//::Main
int main(int argc, char *argv[]) {
	pthread_t thread1;
	char *sharedTask;

	//INITIALIZATION..
	signal(SIGINT, stopHandler);
	initClientSocket(&client);
	createThread(&thread1, receiveFromServer, &client);
	getSharedMemory(sharedTask, 32, 5678);
	sendProcessID();

	initSpace(argc,argv);
	kill(serverPID, SIGUSR2); //Tell server we are ready, must be below initSpace()
	initScreen();
	initRover();

//	newProcess(socketProcessFcn,emptyFcn,(void *)&rover, NULL);

	//CYCLING...
	while(1) sleep(1);

	//DEINITIALIZATION...
	endwin();

	return 0;
}

static void stopHandler()
{

	close(client.initSocket_fd);
        close(client.sharedSocket_fd);

	endwin();
	printf("User wish ext program!\n");

	kill(serverPID, SIGINT);
	kill(getpid(), SIGKILL);
}

static void initScreen()
{
	initscr();
        mapa = newwin(SPACE_SIZE,2*SPACE_SIZE, 2,2);
	printw("Global map view");

	repaintMap();
	box(mapa, 0, 0);
        wrefresh(mapa);
        refresh();

}

static void repaintMap()
{
        for(int i=0; i < SPACE_SIZE; i++){
                for(int j=0; j < SPACE_SIZE*2; j++)
                {
                        mvwaddch(mapa,i, j,Space[i][j]);
                }
                waddch(mapa, '\n');
        }
}

//::Function definitions
static void *receiveFromServer(socketArgs_t *args)
{
	uint16_t word;
        Data_Packet p;
        int recs =0;

        while(1) {
                recs = receiveSocket(&args->initSocket_fd, (char*)&word, 2);

                if(word == PACKET_MARK) {
                        //Readpacket header and CRC
                        recs = receiveSocket(&args->initSocket_fd, (char*)&p+2, sizeof(Data_Packet) - 3);
			if(p.header.data_len > 0) {

				//Dangerous- Dynamic allocaton!!!!
                        	p.data = malloc(p.header.data_len * sizeof(uint8_t));
                        	if(p.data == NULL) printf("ERROR ALLOCATION\n");
	                       	//Read data
                        	receiveSocket(&args->initSocket_fd, (char*)p.data, p.header.data_len);
				//Calc CRC
                        	uint8_t calcCRC = calc_crc8((uint8_t *)p.data, p.header.data_len);

                        	if(p.crc == calcCRC)
                                //PARSING PACKETS
                                switch(p.header.type) {
					case packet_type_move : updateRover(*p.data); break;
                                	case packet_type_pid  : setServerPID(p.data); break;
				}

				free(p.data);   //Uvolinit pamat
                        }
			else {
				switch(p.header.type) {
                                        case packet_type_lidar_data: sendLidarData(); break;
                                }
			}  //PACKET WITHOUT DATA
                } //PACKET MARK NOT FOUND
        } // REPEAT

        close(args->initSocket_fd);
        close(args->sharedSocket_fd);

}

static void setServerPID(uint8_t *data)
{
	uint16_t pID = data[0] | data[1] << 8;
	serverPID = pID;
}

static void updateRover(uint8_t move)
{
	switch(move) {

		case up : nextMove(-1, 0); break;
		case down : nextMove(1, 0); break;
		case right : nextMove(0, 1); break;
		case left : nextMove(0, -1); break;
		default : printf("Invalid movement command!\n");
	}
	updateLidarData(rover);
}

static void nextMove(int x, int y)
{
	Point next;
	next.x = rover.x +x;
	next.y = rover.y +y;

	if(isFree(next)) {
		rover.x += x;
		rover.y += y;
		mvwaddch(mapa, rover.x, rover.y, '*');
		wrefresh(mapa);
	}
}


static void emptyFcn(){};

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
        	sendSocket(&client.initSocket_fd, (char *)&p, packet_size -1);
        	sendSocket(&client.initSocket_fd, (char *)p.data, sizeof(pid_t));
}

static void sendLidarData()
{
	uint16_t dataSize = MAP_SIZE * MAP_SIZE;
	Data_Packet p;
	p.header.mark = PACKET_MARK;
	p.header.data_len = dataSize;
	p.header.type = packet_type_lidar_data;

	p.data = LidarData;
	p.crc = calc_crc8((uint8_t *)&LidarData, dataSize);

	int sendhead = sendSocket(&client.initSocket_fd, (char *)&p, sizeof(Data_Packet) - 1);
	int senddata = sendSocket(&client.initSocket_fd, (char *)p.data, dataSize);

/*	printf("senthead = %d\n", sendhead);
	printf("sentdata = %d\n", senddata);
*/
}

static void sendSimplePacket(uint8_t packet_type)
{
        uint16_t packet_size = sizeof(Data_Packet);
        Data_Packet  p;

        p.header.mark          = PACKET_MARK;
        p.header.data_len      = 0;
        p.header.type          = packet_type;
        p.crc                  = 0;

	sendSocket(&client.initSocket_fd, (char *)&p, packet_size -1);
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

static void initRover()
{
	rover.x = start.x+1;
	rover.y = start.y+1;
	updateLidarData(rover);
}
