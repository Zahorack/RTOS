#include "xspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ncurses.h>


static void nextMove(int x, int y, Point *, char);
static void repaintMap();
static void addBlock(Point);
static WINDOW *scr = NULL;

char Space[SPACE_SIZE][SPACE_SIZE*2];
char Map[MAP_SIZE][MAP_SIZE];
char LidarData[MAP_SIZE * MAP_SIZE];

Point start;
Point goal;

void initSpace(int count, char *argv[]){

	Point block;

	start.x = LIDAR_RANGE+1;
	start.y = LIDAR_RANGE+1;

	initscr();
	scr = newwin(SPACE_SIZE+2,2*SPACE_SIZE+2, 2,2);
	cbreak();
	noecho();
//	keypad(scr, true);

	printw("Initialization of space");
	wrefresh(scr);
	refresh();

if(count > 1) {
	goal.x = LIDAR_RANGE + atoi(argv[1]);
	goal.y = LIDAR_RANGE + atoi(argv[2]);
}
else {
	goal.x = SPACE_SIZE - 2*LIDAR_RANGE;
	goal.y = SPACE_SIZE - 2*LIDAR_RANGE;

}
	//pripravi cisty priestor
	for(int i=0; i < SPACE_SIZE; i++)
		for(int j=0; j < SPACE_SIZE*2; j++)
		{
			if(i == goal.x && j == goal.y)
				Space[i][j] = 'O';
			if(i < LIDAR_RANGE || j < LIDAR_RANGE || i >= SPACE_SIZE - LIDAR_RANGE || j >= 2*SPACE_SIZE - LIDAR_RANGE)
				Space[i][j] = 'X';
			else
				Space[i][j] = ' ';

		}

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
	mvwprintw(scr,start.x, start.y,"S");
	mvwprintw(scr,goal.x, goal.y,"O");
	wrefresh(scr);


if(count <= 3) { //neboli zadane polohy prekazok z konzoli
	char c;
	mvprintw(1,0, "Move ""Sart point"" with w,a,s,d and set with ""Enter"".");
	while((c = getch()) != '\n') {
		switch(c) {

		case 'w' : nextMove(-1, 0, &start, 'S'); break;
                case 's' : nextMove(1, 0, &start, 'S'); break;
                case 'd' : nextMove(0, 1, &start, 'S'); break;
                case 'a' : nextMove(0, -1, &start,'S'); break;

		}
	}
	mvprintw(1,0, "Move ""End point"" with w,a,s,d and set with ""Enter"".");
	while((c = getch()) != '\n') {
		switch(c) {

		case 'w' : nextMove(-1, 0, &goal, 'O'); break;
                case 's' : nextMove(1, 0, &goal, 'O'); break;
                case 'd' : nextMove(0, 1, &goal, 'O'); break;
                case 'a' : nextMove(0, -1, &goal, 'O'); break;

		}
	}

	mvprintw(1,0, "Move ""Block point"" with w,a,s,d and set with ""Enter"". To exit blocks assigment press ""q"". ");

	while((c = getch()) != 'q') {
		block.x = block.y = LIDAR_RANGE +1;
		while((c = getch()) != '\n') {
			switch(c) {

			case 'w' : nextMove(-1, 0, &block, 'X'); break;
                	case 's' : nextMove(1, 0, &block, 'X'); break;
                	case 'd' : nextMove(0, 1, &block, 'X'); break;
               	 	case 'a' : nextMove(0, -1, &block, 'X'); break;

			}
		}
		addBlock(block);
	}
}
	clear();
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
		Space[point->x][point->y] = ' ';
                point->x += x;
                point->y += y;
                Space[point->x][point->y] = ch;
		repaintMap();
                wrefresh(scr);
        }
}

static void addBlock(Point block) {

	Point erase;
	if(isAvailable(block)) {
		for(int l = block.x - BLOCK_RANGE; l < block.x + BLOCK_RANGE; l++){
			for(int m = block.y - BLOCK_RANGE; m < block.y + BLOCK_RANGE; m++){
				erase.x = l;
				erase.y = m;
				if(isAvailable(erase))
					Space[l][m] = 'X';
			}
		}
	}
}

static void repaintMap()
{
        for(int i=0; i < SPACE_SIZE; i++){
                for(int j=0; j < SPACE_SIZE*2; j++)
                {
                        mvwaddch(scr, i+1, j+1,Space[i][j]);
                }
                waddch(scr, '\n');
        }
}


void updateMap(Point actual){

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
	for(int i = actual.x - ln, x = - ln; x <= ln; i++, x++){
		for(int j = actual.y - ln, y = -ln; y <= ln; j++, y++){
			if((x*x + y*y) <= ln*ln ) {
				LidarData[iterator] = Space[i][j];
			}
			else {
				LidarData[iterator] = 'n';
			}

			iterator++;
		}
	}
}

void printLidarData()
{
 #ifdef SPACE_DEBUG
	int iterator = 0;
	int ln = MAP_SIZE;
        for(int i =0; i < ln; i++){
                for(int j = 0; j < ln; j++){
                        printf("%c",LidarData[iterator]);
			iterator++;
                }
		printf("\n");
        }
 #endif
}


int isAvailable(Point point){
	return (point.x < SPACE_SIZE && point.x >= 0 && point.y < SPACE_SIZE*2 && point.y >= 0);
}

int isFree(Point point){
	return (Space[point.x][point.y] != 'X');
}
