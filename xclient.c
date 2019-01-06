
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
#include "xnavigation.h"

//::Function declarations
static void emptyFcn();
static void initClientSocket(socketArgs_t *args);
static void sendLidarData();
static void *receiveFromServer();
static void initRover();
static void nextMove(int, int);
static void updateRover(uint8_t *);
static void updateDeltaMove(uint8_t *);
static void initScreen();
static void repaintMap();
static void setServerPID(uint8_t *);
static void stopHandler();
static void sendProcessID();
static void sendCoordinates();
static void sendSimplePacket(uint8_t );
static void runServer();
static void mazeNavigation();

//::Global variables
socketArgs_t client;
coordinates_t coords;
Point rover;
pid_t serverPID;
uint8_t *sharedTask;
WINDOW *space = NULL;

//::Main
int main(int argc, char *argv[]) {
	pthread_t thread1;


	//INITIALIZATION..
	signal(SIGINT, stopHandler);
	initSpace(argc,argv);

	/* Create Server interaction only if needed */
	if(navigation.local.type == LidarReaction) {
		createProcess(runServer, emptyFcn, NULL, NULL);
		/* Comunication with server*/
		printf("\nCreating server process..\n");
		sleep(3);

		initClientSocket(&client);
		createThread(&thread1, receiveFromServer, &client);
		sendProcessID();
		printf("Socket comunication available...\n");

		sleep(1);
		sharedTask = getSharedMemory(sizeof(navigation_t), 5678);
		memset(sharedTask, 0, sizeof(navigation_t));
		memcpy(sharedTask, &navigation, sizeof(navigation_t));
		printf("Shared memory available..\n");

		sleep(1);
		printf("Server ready!");
		kill(serverPID, SIGUSR2); //Tell server we are ready, must be below initSpace()
	}

	initScreen();
	initRover();

	if(navigation.local.type == MazeReaction) {
		Space[goalPoint.y][goalPoint.x] = ' ';
		initTimer(mazeNavigation, SIGUSR1,0, 10000000);
	}

	if(navigation.local.maze.algorithm.type == Compare) {
		endwin();
		createProcess(mazeRightHandPrincipleAlgorithmStatictic, emptyFcn, NULL, NULL);
		createProcess(mazeLeftHandPrincipleAlgorithmStatictic, emptyFcn, NULL, NULL);
	}

	//CYCLING...
	while(1) sleep(1);

	//DEINITIALIZATION...
	stopHandler();

	return 0;
}

static void mazeNavigation()
{

	switch(navigation.local.maze.algorithm.type) {
		case DefaultMazeAlgorithm: mazeDefaultAlgorithm(space); break;
		case RightHandAlgorithm:   mazeHandPrincipleAlgorithm(space, right); break;
		case LeftHandAlgorithm:	   mazeHandPrincipleAlgorithm(space, left); break;
		default : break;

	}
}


static void runServer()
{

//	system("xterm -x bash -c './server'; 'geometry = 50x50 10+10'; exec bash");
	system("xterm -e './server' -- 'geometry=50x50'");
//	system("xterm -e bash -c '--geometry=50x50+100+120; ./server; exec bash'");
}

static void stopHandler()
{

	close(client.initSocket_fd);
        close(client.sharedSocket_fd);
	endwin();
	printf("User wish to exit program!\n");

	kill(serverPID, SIGINT);
	kill(getpid(), SIGKILL);
}

static void initScreen()
{
	clear();
	endwin();
	initscr();
        space = newwin(SPACE_SIZE+2,2*SPACE_SIZE+2, 2,2);


        if(!has_colors()) {
                printw("Terminla has no colors");
                exit(EXIT_FAILURE);
        }
        start_color();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	printw("Global map view");
	attroff(COLOR_PAIR(1));

	printSpace(space);
//	box(mapa, 0, 0);
//      wrefresh(space);
//        refresh();

}

static void repaintMap()
{
        for(int y=0; y < SPACE_SIZE; y++){
                for(int x=0; x < SPACE_SIZE*2; x++)
                {
                        mvwaddch(space, y, x, Space[y][x]);
                }
                waddch(space, '\n');
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
					case packet_type_move : updateRover(p.data); break;
                                	case packet_type_pid  : setServerPID(p.data); break;
					case packet_type_delta_move : updateDeltaMove(p.data); break;
				}

				free(p.data);   //Uvolinit pamat
                        }
			else {
				switch(p.header.type) {
                                        case packet_type_lidar_data: sendLidarData(); break;
					case packet_type_coordinates : sendCoordinates(); break;
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

static void updateDeltaMove(uint8_t *data)
{
	int16_t dx = ntohs(data[0] | data[1] << 8);
	int16_t dy = ntohs(data[2] | data[3] << 8);

	nextMove(dx, dy);

}

static void updateRover(uint8_t *move)
{
	switch(*move) {

		case up : nextMove(0, -1); break;
		case down : nextMove(0, 1); break;
		case right : nextMove(1, 0); break;
		case left : nextMove(-1, 0); break;
		default : printf("Invalid movement command!\n");
	}
}

static void nextMove(int x, int y)
{
	Point next;
	next.x = rover.x +x;
	next.y = rover.y +y;

	if(isFree(next)) {
		rover.x += x;
		rover.y += y;
		mvwprintw(space, rover.y, rover.x*2, " *");
		wrefresh(space);
	}
	updateLidarData(rover);
}


static void emptyFcn(){}

static void sendCoordinates()
{

	coords.start = startPoint;
	coords.actual = rover;
	coords.goal = goalPoint;
/*
	printf("goalx :%d goaly: %d\n", coords.goal.x, coords.goal.y);
        printf("actualx :%d actualy : %d\n", coords.actual.x, coords.actual.y);

*/	Data_Packet p;
        p.header.mark          = PACKET_MARK;
        p.header.data_len      = sizeof(coordinates_t);
        p.header.type          = packet_type_coordinates;
        p.crc                  = calc_crc8((uint8_t *)&coords, p.header.data_len);

        sendSocket(&client.initSocket_fd, (char *)&p, sizeof(Data_Packet) -1);
        sendSocket(&client.initSocket_fd, (char *)&coords, p.header.data_len);

	//GET NAVIGATION
	//NAVIGUJ MA......!!!
	sendSimplePacket(packet_type_navigation);
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
	rover.x = startPoint.x;
	rover.y = startPoint.y;
	updateLidarData(rover);
}
