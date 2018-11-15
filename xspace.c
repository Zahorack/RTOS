#include "xspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

char Space[SPACE_SIZE][SPACE_SIZE];
char Map[MAP_SIZE][MAP_SIZE];

void initSpace(int count, char *argv[]){

	Point block;
	Point goal;

	goal.x = atoi(argv[1]);
	goal.y = atoi(argv[2]);

	//pripravi cisty priestor
	for(int i=0; i < SPACE_SIZE; i++)
		for(int j=0; j < SPACE_SIZE; j++)
		{
			Space[i][j] = '-';
			if(i == goal.x && j == goal.y)
				Space[i][j] = 'O';

		}

	//umiestni prekazky
	for(int i = 3; i < count; i = i+2){
		block.x = atoi(argv[i]);
		block.y = atoi(argv[i+1]);

		for(int l = block.x - BLOCK_RANGE; l < block.x + BLOCK_RANGE; l++){
			for(int m = block.y - BLOCK_RANGE; m < block.y + BLOCK_RANGE; m++){
				Space[l][m] = 'X';
			}
		}

	}

#ifdef SPACE_DEBUG
	for(int i=0; i < SPACE_SIZE; i++){
		for(int j=0; j < SPACE_SIZE; j++)
		{
			printf("%c",Space[i][j]);
		}
		printf("\n");
	}
#endif
}


void updateMap(Point actual){

	int ln = LIDAR_RANGE;
	for(int i = actual.x - ln, x = - ln; x < ln; i++, x++){
		for(int j = actual.y - ln, y = -ln; y < ln; j++, y++){
			 if((x*x + y*y) <= ln*ln )
				Map[x+ln][y+ln] = Space[i][j];
			else
				Map[x+ln][y+ln] = 'n';
		}
	}
	printMap();
}


void printMap(){

 #ifdef SPACE_DEBUG
	int ln = MAP_SIZE;
        for(int i =0; i < ln; i++){
                for(int j = 0; j < ln; j++){
                         printf("%c",Map[i][j]);
                }
		printf("\n");
        }
 #endif
}

int isAvailable(Point point){
	return (point.x < SPACE_SIZE && point.x >= 0 && point.y < SPACE_SIZE && point.y >= 0);
}

int isFree(Point point){
	return (Space[point.x][point.y] == '-');
}
