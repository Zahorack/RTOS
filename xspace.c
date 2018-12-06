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
static void repaintMap();
static void addBlock(Point);
static void localLidarInitSpace(int, char**);
static void print_menu(WINDOW *menu_win, int highlight, char **list, int list_size);
static uint8_t selectMenu(char **list, int size);
static void localMazeInitSpace();
static void repaintMaze();

static	WINDOW *menu_win = NULL;
static WINDOW *scr = NULL;

unsigned char Space[SPACE_SIZE][SPACE_SIZE*2];
unsigned char Map[MAP_SIZE][MAP_SIZE];
unsigned char LidarData[MAP_SIZE * MAP_SIZE];
unsigned char Maze[SPACE_SIZE][SPACE_SIZE];

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
	"Mars",
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


	startPoint.x = LIDAR_RANGE+1;
	startPoint.y = LIDAR_RANGE+1;

	//::Navigation select

	mvprintw(4,3,"Select type of navigation"); refresh();

	memset(&navigation, 0, sizeof(navigation_t));
	navigation.type = selectMenu(NavigationTypes, n_NavigationTypes);

	switch(navigation.type) {

		case 1: mvprintw(4,3,"Select type of Local navigation"); refresh();
			navigation.local.type = selectMenu(LocalNavigations, n_LocalNavigations);
			switch(navigation.local.type) {
				case 1: mvprintw(4,3,"Select Lidar Algorithm to be used"); refresh();
					navigation.local.lidar.type = selectMenu(LidarReactions, n_LidarReactions);
					switch(navigation.local.lidar.type) {
						case 2: navigation.local.lidar.algorithm.bug.type = selectMenu(BugAlgorithms, n_BugAlgorithms); break;
					}
					break;
				case 2: mvprintw(4,3,"Select Maze Algorithm to be used"); refresh();
					navigation.local.maze.algorithm.type = selectMenu(MazeAlgorithms, n_MazeAlgorithms); break;
			}
			break;

		case 2: mvprintw(4,3,"Select type of Local navigation"); refresh();
			navigation.global.type = selectMenu(GlobalNavigations, n_GlobalNavigations);
			break;
	}

	clear();
	refresh();
	endwin();

	//-------------------------------------------------------


	initscr();
	scr = newwin(SPACE_SIZE+2,2*SPACE_SIZE+2, 2,2);
	cbreak();
	noecho();

	printw("Initialization of space");
	wrefresh(scr);
	refresh();

	switch(navigation.local.type) {
		case 1: localLidarInitSpace(count, argv); break;
		case 2: localMazeInitSpace(); break;
	}



	wclear(scr);
	wrefresh(scr);
	refresh();
	endwin();

}

static void localMazeInitSpace()
{
	initMaze();

	//repaintMaze();

}

/*
static void repaintMaze()
{
        for(int y = 0; y < SPACE_SIZE; y++){
                for(int x = 0; x < SPACE_SIZE; x++)
                {
//                        printf("%c", maze[y][x]);
			if(maze[y][x] != ' ')
			mvwprintw(scr, y+1, x+1,"X");
			else
				mvwprintw(scr, y+1, x+1, " ");
                }
//                printf("\n");
		wprintw(scr, "\n");
        }
	wrefresh(scr);
}
*/

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

	if(count > 1) {
		goalPoint.x = LIDAR_RANGE + atoi(argv[1]);
		goalPoint.y = LIDAR_RANGE + atoi(argv[2]);
	}
	else {
		goalPoint.x = 2*SPACE_SIZE - 2*LIDAR_RANGE;
		goalPoint.y = SPACE_SIZE - 2*LIDAR_RANGE;
	}

	//pripravi cisty priestor
	for(int i=0; i < SPACE_SIZE; i++)
		for(int j=0; j < SPACE_SIZE*2; j++)
		{
			if(i < LIDAR_RANGE || j < LIDAR_RANGE || i >= SPACE_SIZE - LIDAR_RANGE || j >= 2*SPACE_SIZE - LIDAR_RANGE)
				Space[i][j] = 'X';
			else
				Space[i][j] = ' ';

		}

	Space[startPoint.y][startPoint.x] = 'S';
	Space[goalPoint.y][goalPoint.x] = 'O';

	//umiestni prekazky
	for(int i = 3; i < count; i = i+2){
		block.x = LIDAR_RANGE + atoi(argv[i]);
		block.y = LIDAR_RANGE + atoi(argv[i+1]);
		addBlock(block);
	}


	#ifdef SPACE_DEBUG
	for(int i=0; i < SPACE_SIZE; i++){
		for(int j=0; j < SPACE_SIZE*2; j++)
		{
			mvwaddch(scr, i+1, j+1,Space[i][j]);
		}
		waddch(scr, '\n');
	}
	#endif


	box(scr,0,0);
	mvwaddch(scr,startPoint.x, startPoint.y,'S');
	mvwaddch(scr,goalPoint.x, goalPoint.y,'O');

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

			case 'w' : nextMove(0, -1, &goalPoint, 'O'); break;
	                case 's' : nextMove(0, 1, &goalPoint, 'O'); break;
	                case 'd' : nextMove(1, 0, &goalPoint, 'O'); break;
	                case 'a' : nextMove(-1, 0, &goalPoint, 'O'); break;
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
		repaintMap();
                wrefresh(scr);
        }
}

static void addBlock(Point block) {

	Point erase;
	if(isAvailable(block)) {
		for(int x = block.x - BLOCK_RANGE; x < block.x + BLOCK_RANGE; x++){
			for(int y = block.y - BLOCK_RANGE; y < block.y + BLOCK_RANGE; y++){
				erase.x = x;
				erase.y = y;
				if(isAvailable(erase) && Space[erase.y][erase.x] != 'O')
					Space[y][x] = 'X';
			}
		}
	}
}

static void repaintMap()
{
        for(int y = 0; y < SPACE_SIZE; y++){
                for(int x = 0; x < SPACE_SIZE*2; x++)
                {
                        mvwaddch(scr, y+1, x+1,Space[y][x]);
                }
                waddch(scr, '\n');
        }
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
	return (point.y < SPACE_SIZE && point.x >= 0 && point.x < SPACE_SIZE*2 && point.y >= 0);
}

int isFree(Point point)
{
	return (Space[point.y][point.x] != 'X');
}

