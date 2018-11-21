#ifndef XSPACE_H_
#define XSPACE_H_


#define BLOCK_RANGE	2
#define SPACE_SIZE      70
#define LIDAR_RANGE	10
#define MAP_SIZE	(LIDAR_RANGE*2 +1)
#define ROOM_SIZE	SPACE_SIZE - LIDAR_RANGE
#define SPACE_DEBUG

extern char Space[SPACE_SIZE][SPACE_SIZE];
extern char Map[MAP_SIZE][MAP_SIZE];
extern char LidarData[MAP_SIZE * MAP_SIZE];

typedef struct {

        int x;
        int y;
}Point;

void initSpace(int count, char **argv);
void updateMap(Point);
void printMap();
void printLidarData();
void updateLidarData(Point);

int  isAvailable(Point);
int  isFree(Point);

#endif
