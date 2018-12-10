#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xspace.h"
#include "xmaze.h"
#include "xtimer.h"
#include <ncurses.h>


static void ShowMaze(const char *maze, int width, int height) {
   int x, y;

   for(y = 0; y < height; y++) {
      for(x = 0; x < width; x++) {
         switch(maze[y * width + x]) {
         case 'X':  printf("\u2588\u2588");  break;
         case '*':  printf(" *");  break;
         default: printf("  "); break;

//	printf("%c", maze[y*x]);
	}
      }
      printf("\n");
   }
}

static void CarveMaze(char *maze, int width, int height, int x, int y) {

   int x1, y1;
   int x2, y2;
   int dx, dy;
   int dir, count;

   dir = rand() % 4;
   count = 0;
   while(count < 4) {
      dx = 0; dy = 0;
      switch(dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
      }
      x1 = x + dx;
      y1 = y + dy;
      x2 = x1 + dx;
      y2 = y1 + dy;
      if(   x2 > 0 && x2 < width && y2 > 0 && y2 < height
         && maze[y1 * width + x1] == 'X' && maze[y2 * width + x2] == 'X') {
         maze[y1 * width + x1] = ' ';
         maze[y2 * width + x2] = ' ';
         x = x2; y = y2;
         dir = rand() % 4;
         count = 0;
      } else {
         dir = (dir + 1) % 4;
         count += 1;
      }
   }

}

static void GenerateMaze(char *maze, int width, int height)
{
   	int x, y;
   	/* Initialize the maze. */
   	for(x = 0; x < width * height; x++) {
      		maze[x] = 'X';
   	}
   	maze[1 * width + 1] = ' ';

   	/* Seed the random number generator. */
   	srand(time(0));

   	/* Carve the maze. */
   	for(y = 1; y < height; y += 2) {
      		for(x = 1; x < width; x += 2) {
         		CarveMaze(maze, width, height, x, y);
      		}
   	}

   	/* Set up the entry and exit. */
   	maze[0 * width + 1] = 'S';
   	maze[(height - 1) * width + (width - 2)] = 'G';

	startPoint.x = 1;
	startPoint.y = 0;

	goalPoint.x = (height - 1)*width;
	goalPoint.y = width -2;

	/* Fill global 2D array Maze */
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			Space[y][x] = maze[y * width + x];
		}
	}
}

void initMaze() {

   char *maze;
	int width = SPACE_SIZE;
	int height= SPACE_SIZE;

   /* Get and validate the size. */
   if(!(width %2)) {
	width -=1;
   }
   if(!(height %2)) {
	height -=1;
   }

   if(width < 7 || height < 7) {
      printf("error: illegal maze size\n");
      exit(EXIT_FAILURE);
   }

   /* Allocate the maze array. */
   maze = (char*)malloc(width * height * sizeof(char));
   if(maze == NULL) {
      printf("error: not enough memory\n");
      exit(EXIT_FAILURE);
   }

   /* Generate and display the maze. */
   GenerateMaze(maze, width, height);
  // ShowMaze(maze, width, height);


   /* Clean up. */
   free(maze);

}
