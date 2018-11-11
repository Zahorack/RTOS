#include "xspace.h"
#include <stdio.h>
#include <stdlib.h>

char Space[SPACE_SIZE][SPACE_SIZE];

void initSpace(int count, char *argv[]){

	Point block;

	//pripravi cisty priestor
	for(int i=0; i < SPACE_SIZE; i++)
		for(int j=0; j < SPACE_SIZE; j++)
		{
			Space[i][j] = '-';
		}

	//umiestni prekazky
	for(int i=1; i < count; i = i+2){
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
