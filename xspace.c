#include "xspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ncurses.h>

char Space[SPACE_SIZE][SPACE_SIZE];
char Map[MAP_SIZE][MAP_SIZE];
char LidarData[MAP_SIZE * MAP_SIZE];

void initSpace(int count, char *argv[]){

	Point block;
	Point goal;

	goal.x = LIDAR_RANGE + atoi(argv[1]);
	goal.y = LIDAR_RANGE + atoi(argv[2]);

	//pripravi cisty priestor
	for(int i=0; i < SPACE_SIZE; i++)
		for(int j=0; j < SPACE_SIZE; j++)
		{
			if(i == goal.x && j == goal.y)
				Space[i][j] = 'O';
			if(i < LIDAR_RANGE || j < LIDAR_RANGE || i >= SPACE_SIZE - LIDAR_RANGE || j >= SPACE_SIZE - LIDAR_RANGE)
				Space[i][j] = 'X';
			else
				Space[i][j] = '-';

		}

	//umiestni prekazky
	for(int i = 3; i < count; i = i+2){
		block.x = LIDAR_RANGE + atoi(argv[i]);
		block.y = LIDAR_RANGE + atoi(argv[i+1]);

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
	for(int i = actual.x - ln, x = - ln; x <= ln; i++, x++){
		for(int j = actual.y - ln, y = -ln; y <= ln; j++, y++){
			 if((x*x + y*y) <= ln*ln )
				Map[x+ln][y+ln] = Space[i][j];
			else
				Map[x+ln][y+ln] = 'n';
		}
	}
//	printMap();
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
