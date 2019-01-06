#include "xnavigation.h"
#include "xspace.h"
#include <ncurses.h>
#include "stdint.h"

/*                               ____________
 *                              |            |
 *                              | NAVIGATION |
 *                              |____________|
 *                                    |
 *                    ________________|______________
 *                   |                               |
 *               ____|___                         ___|___
 *              | Global |                       | Local |
 *              |________|                       |_______|
 *                 |                     ____________|____________
 *                                      |            |            |
 *                                     Bug       Wandering       Maze
 *                                  algorithm      point
 *
 *
 *
 */

static uint8_t moveFromOrientation(uint8_t orientation, uint8_t move);
//static uint8_t nextCell(WINDOW *,uint8_t move);


void mazeRightHandPrincipleAlgorithmStatictic()
{
        static int dir = up, count =0;
        static int dx, dy;
        static int forward = 1;
	static Point actual;
	static uint8_t lastMove = up;
	static uint8_t orientation = down;
	static uint8_t lastOrientation = 0;

	static int init = 1;
	if(init) {
		actual.x = startPoint.x;
		actual.y = startPoint.y;
		init = 0;
	}


	int steps =0;
	int way =0;
        while(actual.x != goalPoint.x || actual.y != goalPoint.y) {


		if(lastMove != up) {
			orientation = moveFromOrientation(orientation,lastMove);
			lastMove = up;
		}

                dx = 0; dy = 0;
                switch(moveFromOrientation(orientation, dir)) {
	                case up: dy = -1; break;
	                case down:dy = 1; break;
	                case right: dx = 1; break;
	                case left: dx = -1; break;
                }

                Point next;
                next.x = actual.x +dx;
                next.y = actual.y +dy;

                if((forward && isFree(next)) || (!forward && wasVisited(next))) {
                        Space[actual.y][actual.x] = forward ? '*' : '#';
                        forward ? setVisited(actual) : setBlind(actual);

			forward ? way++ : way--;
			steps++;

                        actual.x += dx;
                        actual.y += dy;
			lastMove = dir;
                        forward = 1;
                        count = 0;
                        dir = right;
		}
                else {
                        //dir = (dir + 1) % 4;
                        dir++;
			if(dir == 4) dir =0;
			count += 1;
                        if(count > 3) {
                                forward = 0;
                                count = 0;
                        }
                }
	}
	printf("Maze algortimh Right hand principle\n");
	printf("Steps: %d\n", steps);
	printf("Way lenght: %d\n\n", way);
}

void mazeLeftHandPrincipleAlgorithmStatictic()
{
        static int dir = up, count =0;
        static int dx, dy;
        static int forward = 1;
	static Point actual;
	static uint8_t lastMove = up;
	static uint8_t orientation = down;
	static uint8_t lastOrientation = 0;

	static int init = 1;
	if(init) {
		actual.x = startPoint.x;
		actual.y = startPoint.y;
		init = 0;
	}


	int steps =0;
	int way =0;
        while(actual.x != goalPoint.x || actual.y != goalPoint.y) {


		if(lastMove != up) {
			orientation = moveFromOrientation(orientation,lastMove);
			lastMove = up;
		}

                dx = 0; dy = 0;
                switch(moveFromOrientation(orientation, dir)) {
	                case up: dy = -1; break;
	                case down:dy = 1; break;
	                case right: dx = 1; break;
	                case left: dx = -1; break;
                }

                Point next;
                next.x = actual.x +dx;
                next.y = actual.y +dy;

                if((forward && isFree(next)) || (!forward && wasVisited(next))) {
                        Space[actual.y][actual.x] = forward ? '*' : '#';
                        forward ? setVisited(actual) : setBlind(actual);

			forward ? way++ : way--;
			steps++;

                        actual.x += dx;
                        actual.y += dy;
			lastMove = dir;
                        forward = 1;
                        count = 0;
                        dir = left;
		}
                else {
                        //dir = (dir + 1) % 4;
                        dir++;
			if(dir == 4) dir =0;
			count += 1;
                        if(count > 3) {
                                forward = 0;
                                count = 0;
                        }
                }
	}
	printf("Maze algortimh Left hand principle\n");
	printf("Steps: %d\n", steps);
	printf("Way lenght: %d\n\n", way);
}


/* Intuitive algorithm searching path
 *
 *	chod smerom k vychodu
 */

void mazeDefaultAlgorithm(WINDOW *scr)
{
        static int dir =0, count =0;
        static int dx, dy;
        static int forward = 1;
	static Point actual;

	static int init = 1;
	if(init) {
		actual.x = startPoint.x;
		actual.y = startPoint.y;
		init = 0;
	}

        if(actual.x != goalPoint.x || actual.y != goalPoint.y) {

                dx = 0; dy = 0;
                switch(dir) {
                        case 0:  dx = 1;  break;
                        case 1:  dy = 1;  break;
                        case 2:  dx = -1; break;
                        default: dy = -1; break;
              }

                Point next;
                next.x = actual.x +dx;
                next.y = actual.y +dy;

                if((forward && isFree(next)) || (!forward && wasVisited(next))) {
                        Space[actual.y][actual.x] = forward ? '*' : '#';

                        forward ? setVisited(actual) : setBlind(actual);
                        actual.x += dx;
                        actual.y += dy;
                        forward = 1;
                        count = 0;
                        dir = 0;


                        printSpace(scr);
                }
                else {
                        dir = (dir + 1) % 4;
                        count += 1;
                        if(count > 3) {
                                forward = 0;
                                count = 0;
                        }
                }
	}
}

/* Algotithm based on right hand principle */
void mazeHandPrincipleAlgorithm(WINDOW *scr, uint8_t hand)
{
        static int dir = up, count =0;
        static int dx, dy;
        static int forward = 1;
	static Point actual;
	static uint8_t lastMove = up;
	static uint8_t orientation = down;
	static uint8_t lastOrientation = 0;

	static int init = 1;
	if(init) {
		actual.x = startPoint.x;
		actual.y = startPoint.y;
		init = 0;
	}

        if(actual.x != goalPoint.x || actual.y != goalPoint.y) {


		if(lastMove != up) {
			orientation = moveFromOrientation(orientation,lastMove);
			lastMove = up;
		}

                dx = 0; dy = 0;
                switch(moveFromOrientation(orientation, dir)) {
	                case up: dy = -1; break;
	                case down:dy = 1; break;
	                case right: dx = 1; break;
	                case left: dx = -1; break;
                }

                Point next;
                next.x = actual.x +dx;
                next.y = actual.y +dy;

                if((forward && isFree(next)) || (!forward && wasVisited(next))) {
                        Space[actual.y][actual.x] = forward ? '*' : '#';

                        forward ? setVisited(actual) : setBlind(actual);
                        actual.x += dx;
                        actual.y += dy;
			lastMove = dir;
                        forward = 1;
                        count = 0;
                        dir = hand;


                        printSpace(scr);
                }
                else {
                        //dir = (dir + 1) % 4;
                        dir++;
			if(dir == 4) dir =0;
			count += 1;
                        if(count > 3) {
                                forward = 0;
                                count = 0;
                        }
                }
	}
}

static uint8_t moveFromOrientation(uint8_t orientation, uint8_t move)
{
        uint8_t movement;
        int dx =0, dy=0;
        switch(move) {
                case up: dy = -1; break;
                case down:dy = 1; break;
                case right: dx = 1; break;
                case left: dx = -1; break;
        }

        switch(orientation) {
                case up: movement = move; break;
                case down: movement = move -dy + dx; break;
                case right: movement = left - move + dy; break;
                case left: movement = left  - move - dx; break;
        }

                return movement;
}


/*
static void solveMaze()
{
        Point next;
        static int lastDx =0, lasdDy =0;
        static uint8_t invalid = moveSize;
        static uint8_t last = 0;
        static uint8_t newOrientationAlarm =0;

        int recomendedX =0, recomendedY =0;

        if(goalPoint.x > rover.x) recomendedX =1;
        if(goalPoint.y > rover.y) recomendedY =1;

        static uint8_t orientation = 0;

        if(newOrientationAlarm){
                orientation = moveFromOrientation(orientation, last);
                mvprintw(1,1, "new orientation");

        }

        newOrientationAlarm =1;

        if(!nextCell(moveFromOrientation(orientation,right))) {
                if(!nextCell(moveFromOrientation(orientation, up))){
                        if(!nextCell(moveFromOrientation(orientation, down))) {
                                if(!nextCell(moveFromOrientation(orientation, left))) {
                                        mvprintw(1, 15,"noway");
                                }
                                else last = left;
                        }
                        else last = down;
                }
                else newOrientationAlarm = 0;

        }
        else {
                 last = right;
        }

        mvprintw(0,0,"orientation: %d last move: %d", orientation, last);
        refresh();

}


static uint8_t nextCell(WINDOW *scr, uint8_t move)
{
        mvprintw(1,6, "command: %d", move);
        Point next;
        int dx=0, dy=0;
        static int escape = 0;
        static uint8_t lastValid =0;
        static Point escapeVector;
	static Point rover;

        switch(move) {
                case up:   dy = -1; break;
                case down: dy = 1;  break;
                case right:dx = 1;  break;
                case left: dx = -1; break;
        }

        next.x = rover.x +dx;
        next.y = rover.y +dy;

        if(escape == 1 && isFree(next) && move != lastValid) {
                escape =0;
        }

        if(escape == 1) {
                //need to move back
                dy = 0;
                dx = 0;
                switch(lastValid) {
                        case up:   dy = 1; break;
                        case down: dy = -1;  break;
                        case right:dx = -1;  break;
                        case left: dx = 1; break;
                }
                escapeVector.x = dx;
                escapeVector.y = dy;
        }

        next.x = rover.x +dx;
        next.y = rover.y +dy;

        if(isFree(next) && (!wasVisited(next) || escape == 1)) {
                rover.x += dx;
                rover.y += dy;
                setVisited(rover);
                wclear(scr);
                printSpace(scr);
                mvwprintw(scr, rover.y, rover.x*2, " A");
                wrefresh(scr);
                lastValid = move;
                return 1;
        }
        if(move == left) escape = 1;

        return 0;
}
*/

