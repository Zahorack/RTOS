#ifndef XSPACE_H_
#define XSPACE_H_

#define BLOCK_RANGE	3
#define SPACE_SIZE      50
#define LIDAR_RANGE	10
#define MAP_SIZE	LIDAR_RANGE*2
#define SPACE_DEBUG

extern char Space[SPACE_SIZE][SPACE_SIZE];

typedef struct {

        int x;
        int y;
}Point;

void initSpace(int count, char **argv);
void updateMap(Point);
void printMap();


#endif
