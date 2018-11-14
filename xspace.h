#ifndef XSPACE_H_
#define XSPACE_H_

#define BLOCK_RANGE	3
#define SPACE_SIZE      50
//#define SPACE_DEBUG

extern char Space[SPACE_SIZE][SPACE_SIZE];

typedef struct {

        int x;
        int y;
}Point;

void initSpace(int count, char **argv);
#endif
