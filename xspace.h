#ifndef XSPACE_H_
#define XSPACE_H_
#include <stdint.h>
#include <ncurses.h>

#define BLOCK_RANGE	2
#define SPACE_SIZE      60
#define LIDAR_RANGE	9
#define MAP_SIZE	(LIDAR_RANGE*2 +1)
#define ROOM_SIZE	SPACE_SIZE - LIDAR_RANGE
#define SPACE_DEBUG
#define LIDAR_MID	(MAP_SIZE/2)

extern unsigned char Space[SPACE_SIZE][SPACE_SIZE];
extern unsigned char Map[MAP_SIZE][MAP_SIZE];
extern unsigned char LidarData[MAP_SIZE * MAP_SIZE];
extern unsigned char Visited[SPACE_SIZE][SPACE_SIZE];

//extern unsigned char Maze[SPACE_SIZE][SPACE_SIZE];

typedef struct {

        int16_t x;
        int16_t y;
}Point;

extern Point startPoint;
extern Point goalPoint;

enum NavigationTypesEnum {
        LocalNavigation =1,
        GlobalNavigation,
};

enum LocalNavigationsEnum {
        LidarReaction =1,
        MazeReaction,
};

enum GlobalNavigationsEnum {
        DefaultGlobal = 1,
};

enum LidarReactionsEnum {
        DefaultLidarAlgorthm =1,
        BugLidarAlgorithm,
        WanderingLidarAlgorithm,
};

enum MazeAlgorithmsEnum {
        DefaultMazeAlgorithm =1,
        LeftHandAlgorithm,
        RightHandAlgorithm,
        FloodFillAlgorithm,
	Compare,
};


enum move{
        up = 0,
        down,
        right,
        left,

        moveSize
};


void printSpace(WINDOW *);
void initSpace(int count, char **argv);
void updateMap(Point);
void printMap();
void printLidarData();
void updateLidarData(Point);
void setVisited(Point point);
int  wasVisited(Point point);
void setBlind(Point);
int  isBlind(Point);

int  isAvailable(Point);
int  isFree(Point);

#endif
