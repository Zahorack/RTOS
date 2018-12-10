#include "xspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>
#include "xmaze.h"
#include "xnavigation.h"

static void nextMove(int x, int y, Point *, char);
static void addBlock(Point);
static void localLidarInitSpace(int, char**);
static void print_menu(WINDOW *menu_win, int highlight, char **list, int list_size);
static uint8_t selectMenu(char **list, int size);
static void localMazeInitSpace();
static void repaintMaze();

static	WINDOW *menu_win = NULL;
static WINDOW *scr = NULL;

unsigned char Space[SPACE_SIZE][SPACE_SIZE];
unsigned char Map[MAP_SIZE][MAP_SIZE];
unsigned char LidarData[MAP_SIZE * MAP_SIZE];
unsigned char Visited[SPACE_SIZE][SPACE_SIZE];

//unsigned char Maze[SPACE_SIZE][SPACE_SIZE];

Point startPoint;
Point goalPoint;
Point block;

navigation_t navigation;
/*
 *
 *  NavigationType --> GlobalNavigation
 *
 *
 *
 *                 --> LocalNavigation  --> LidarReaction --> Default algorithm
 *                                                        --> Bug algorithm
 *                                                        --> Wandering standpoint
 *                                      --> MazeReaction
 */


char *NavigationTypes[] = {
	"Local naivgation (reactive)",
	"Global navigation",
	"Exit",
};


char *LocalNavigations[] = {
	"Lidar reaction",
	"Maze reaction",

	"Exit",
};

char *GlobalNavigations[] = {
	"Default",
	"Exit",
};

char *LidarReactions[] = {
	"Default algorithm",
	"Bug algorithm",
	"Wandering standpiont",

	"Exit",
};

char *BugAlgorithms[] = {
	"Typ 0",
	"Typ 1",
	"Typ 2",
	"Typ 3",
	"Exit",
};

char *MazeAlgorithms[] = {
	"Default",
	"LeftHand",
	"RightHand",
	"Flood fill",
	"Exit",
};

void initSpace(int count, char *argv[]){

	int n_LocalNavigations = sizeof(LocalNavigations) / sizeof(char *);
	int n_NavigationTypes = sizeof(NavigationTypes) / sizeof(char *);
	int n_GlobalNavigations = sizeof(GlobalNavigations) / sizeof(char *);
	int n_LidarReactions = sizeof(LidarReactions) / sizeof(char *);
	int n_BugAlgorithms = sizeof(BugAlgorithms) / sizeof(char *);
	int n_MazeAlgorithms = sizeof(MazeAlgorithms) / sizeof(char *);
	printf("size :%d ", n_BugAlgorithms);

	initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */

	int startx = 3;
	int starty = 5;
	menu_win = newwin(10, 40, starty, startx);
	keypad(menu_win, TRUE);

	WINDOW *intro = newwin(3,90, 0,0);
	box(intro,0,0);
	mvwprintw(intro, 1,1, "C based Linux program used for simulating robot tracking and pathfinding algorithms");
	mvwprintw(intro, 2,2, "by ""Zahorack"" (Oliver Holly)");

	mvprintw(15, 0, "instructions: Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	wrefresh(intro);


	//::Navigation select

	mvprintw(4,3,"Select type of navigation"); refresh();

	memset(&navigation, 0, sizeof(navigation_t));
	navigation.type = selectMenu(NavigationTypes, n_NavigationTypes);

	switch(navigation.type) {

		case LocalNavigation:
			mvprintw(4,3,"Select type of Local navigation"); refresh();
			navigation.local.type = selectMenu(LocalNavigations, n_LocalNavigations);

			switch(navigation.local.type) {
				case LidarReaction:
					mvprintw(4,3,"Select Lidar Algorithm to be used"); refresh();
					navigation.local.lidar.type = selectMenu(LidarReactions, n_LidarReactions);

					switch(navigation.local.lidar.type) {
						case BugLidarAlgorithm:
							navigation.local.lidar.algorithm.bug.type = selectMenu(BugAlgorithms, n_BugAlgorithms);
							break;
					}
					break;
				case MazeReaction:
					mvprintw(4,3,"Select Maze Algorithm to be used"); refresh();
					navigation.local.maze.algorithm.type = selectMenu(MazeAlgorithms, n_MazeAlgorithms);
					break;
			}
			break;

		case GlobalNavigation: mvprintw(4,3,"Select type of Local navigation"); refresh();
			navigation.global.type = selectMenu(GlobalNavigations, n_GlobalNavigations);
			break;
	}

	clear();
	refresh();
	endwin();

	//-------------------------------------------------------



	switch(navigation.local.type) {
		case 1: localLidarInitSpace(count, argv); break;
		case 2: localMazeInitSpace(); break;
	}




}

static void localMazeInitSpace()
{

	initMaze();
	printf("init maze\n");
	initscr();
	WINDOW *s = newwin(SPACE_SIZE, SPACE_SIZE*2, 2,2);
	printSpace(s);

	endwin();

}

static uint8_t selectMenu(char **list, int size)
{
	uint8_t choice = 0;
	uint8_t highlight = 1;
	int c;

	print_menu(menu_win, highlight, list, size);
	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = size;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == size)
					highlight = 1;
				else
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		print_menu(menu_win, highlight, list, size);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}

	return choice;
}

static void print_menu(WINDOW *menu_win, int highlight, char **list, int size)
{
	int x, y, i;
	x = 2;
	y = 2;

	wclear(menu_win);
	box(menu_win, 0, 0);
	for(i = 0; i < size; ++i)
	{	if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", list[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", list[i]);
		++y;
	}
	wrefresh(menu_win);
}

static void localLidarInitSpace(int count, char *argv[])
{

	initscr();
	scr = newwin(SPACE_SIZE+2,2*SPACE_SIZE+2, 2,2);
	cbreak();
	noecho();

	printw("Initialization of space");
	wrefresh(scr);
	refresh();

	startPoint.x = LIDAR_RANGE+1;
	startPoint.y = LIDAR_RANGE+1;

	if(count > 1) {
		goalPoint.x = LIDAR_RANGE + atoi(argv[1]);
		goalPoint.y = LIDAR_RANGE + atoi(argv[2]);
	}
	else {
		goalPoint.x = SPACE_SIZE - 2*LIDAR_RANGE;
		goalPoint.y = SPACE_SIZE - 2*LIDAR_RANGE;
	}

	//pripravi cisty priestor
	for(int y=0; y < SPACE_SIZE; y++)
		for(int x=0; x < SPACE_SIZE; x++)
		{
			if(x < LIDAR_RANGE || y < LIDAR_RANGE || x >= SPACE_SIZE - LIDAR_RANGE || y >= SPACE_SIZE - LIDAR_RANGE)
				Space[y][x] = 'X';
			else
				Space[y][x] = ' ';

		}

	Space[startPoint.y][startPoint.x] = 'S';
	Space[goalPoint.y][goalPoint.x] = 'G';

	//umiestni prekazky
	for(int i = 3; i < count; i = i+2){
		block.x = LIDAR_RANGE + atoi(argv[i]);
		block.y = LIDAR_RANGE + atoi(argv[i+1]);
		addBlock(block);
	}


	printSpace(scr);

	box(scr,0,0);
	mvwaddch(scr,startPoint.y, startPoint.x,'S');
	mvwaddch(scr,goalPoint.y, goalPoint.x,'O');

	if(count <= 3) { //neboli zadane polohy prekazok z konzoli
		char c;
		mvprintw(1,0, "Move ""Sart point"" with w,a,s,d and set with ""Enter"".");
		while((c = getch()) != '\n') {
			switch(c) {

			case 'w' : nextMove(0, -1, &startPoint, 'S'); break;
	                case 's' : nextMove(0, 1, &startPoint, 'S'); break;
	                case 'd' : nextMove(1, 0, &startPoint, 'S'); break;
	                case 'a' : nextMove(-1, 0, &startPoint,'S'); break;

			}
		}
		mvprintw(1,0, "Move ""End point"" with w,a,s,d and set with ""Enter"".");
		while((c = getch()) != '\n') {
			switch(c) {

			case 'w' : nextMove(0, -1, &goalPoint, 'G'); break;
	                case 's' : nextMove(0, 1, &goalPoint, 'G'); break;
	                case 'd' : nextMove(1, 0, &goalPoint, 'G'); break;
	                case 'a' : nextMove(-1, 0, &goalPoint, 'G'); break;
			}
		}

		mvprintw(1,0, "Move ""Block point"" with w,a,s,d and set with ""Enter"". To exit blocks assigment press ""q"". ");

		while((c = getch()) != 'q') {
			block.x = block.y = LIDAR_RANGE +1;
			while((c = getch()) != '\n') {
				switch(c) {

				case 'w' : nextMove(0, -1, &block, 'X'); break;
	                	case 's' : nextMove(0, 1, &block, 'X'); break;
	                	case 'd' : nextMove(1, 0, &block, 'X'); break;
	               	 	case 'a' : nextMove(-1, 0, &block, 'X'); break;
				}
			}
			addBlock(block);
		}
	}

	wclear(scr);
	wrefresh(scr);
	refresh();
	endwin();


}

static void nextMove(int x, int y, Point *point, char ch)
{
        Point next;
        next.x = point->x +x;
        next.y = point->y +y;

        if(isFree(next)) {
		Space[point->y][point->x] = ' ';
                point->x += x;
                point->y += y;
                Space[point->y][point->x] = ch;
		wclear(scr);
		printSpace(scr);
        }
}

static void addBlock(Point block) {

	Point erase;
	if(isAvailable(block)) {
		for(int y = block.y - BLOCK_RANGE; y < block.y + BLOCK_RANGE; y++){
			for(int x = block.x - BLOCK_RANGE; x < block.x + BLOCK_RANGE; x++){
				erase.x = x;
				erase.y = y;
				if(isAvailable(erase) && Space[erase.y][erase.x] != 'G')
					Space[y][x] = 'X';
			}
		}
	}
}

void printSpace(WINDOW *scr)
{

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	int ln = SPACE_SIZE;
	if(!(ln %2)) {
		ln = ln -1;
	}
	wmove(scr, 0,0);
	for(int y=0; y < ln; y++){
		for(int x=0; x < ln; x++) {
			switch(Space[y][x]) {
				case 'X': waddch(scr,' ' | A_REVERSE);
					  waddch(scr,' ' | A_REVERSE);
					  break;
				case '*': wattron(scr, COLOR_PAIR(1));
					  wprintw(scr," ");
					  wprintw(scr, "%c",Space[y][x]);
					  wattroff(scr,COLOR_PAIR(1));
					  break;
				case '#': wattron(scr, COLOR_PAIR(2));
					  wprintw(scr," ");
					  wprintw(scr, "%c",Space[y][x]);
					  wattroff(scr, COLOR_PAIR(2));
					  break;
				default:  waddch(scr,' ');
					  waddch(scr,Space[y][x]);
					  break;
			}
		}
		waddch(scr, '\n');
	}
	wrefresh(scr);
}


void updateMap(Point actual)
{
	int ln = LIDAR_RANGE;
	for(int i = actual.x - ln, x = - ln; x <= ln; i++, x++){
		for(int j = actual.y - ln, y = -ln; y <= ln; j++, y++){
			 if((x*x + y*y) <= ln*ln )
				Map[x+ln][y+ln] = Space[i][j];
			else
				Map[x+ln][y+ln] = 'n';
		}
	}
}

void updateLidarData(Point actual)
{
	int ln = LIDAR_RANGE;
	int iterator = 0;
	for(int y = actual.y - ln, y1 = -ln; y <= actual.y +ln; y++, y1++){
		for(int x = actual.x - ln, x1 = -ln; x <= actual.x + ln; x++, x1++){
			if((x1*x1 + y1*y1) <= ln*ln ) {
				LidarData[iterator] = Space[y][x];
			}
			else {
				LidarData[iterator] = 'n';
			}

			iterator++;
		}
	}
}

int isAvailable(Point point)
{
	return (point.y < SPACE_SIZE && point.x >= 0 && point.x < SPACE_SIZE && point.y >= 0);
}

int isFree(Point point)
{
	return (isAvailable(point) && Space[point.y][point.x] == ' ');
}

void setVisited(Point point)
{
	Visited[point.y][point.x] = 'v';
}

void setBlind(Point point)
{
	Visited[point.y][point.x] = 'b';
}

int isBlind(Point point)
{
	return (Visited[point.y][point.x] == 'b');
}

int wasVisited(Point point)
{
	return (Visited[point.y][point.x] == 'v');
}
