#ifndef XNAVIGATION_H_
#define XNAVIGATION_H_

//#include "xspace.h"
#include <ncurses.h>
#include <stdint.h>
#include "xspace.h"

typedef struct {
	Point start;
	Point actual;
	Point goal;

} coordinates_t;


void mazeDefaultAlgorithm(WINDOW *);
void mazeHandPrincipleAlgorithm(WINDOW *, uint8_t);
void mazeRightHandPrincipleAlgorithmStatictic();
void mazeLeftHandPrincipleAlgorithmStatictic();


//::ALgorithm specification
typedef struct {
	uint8_t type;
}__attribute__((packed)) bugAlg_t;


//::Algorithms
typedef struct {
	uint8_t type;

}__attribute__((packed)) mazeAlgorithm_t;

typedef struct {
	uint8_t basic;
	bugAlg_t bug;

}__attribute__((packed)) lidarAlgorithm_t;


//::Map/space variant
typedef struct {
	uint8_t type;
	lidarAlgorithm_t algorithm;

}__attribute__((packed)) lidarReaction_t;

typedef struct {
	uint8_t state;
	mazeAlgorithm_t algorithm;
}__attribute__((packed)) mazeReaction_t;


//::Global / Local
typedef struct {
	uint8_t type;
}__attribute__((packed)) globalNav_t;

typedef struct {
	uint8_t type;
	lidarReaction_t lidar;
	mazeReaction_t maze;

}__attribute__((packed)) localNav_t;


//::Navigation
typedef struct {
	uint8_t type;
	globalNav_t global;
	localNav_t local;

}__attribute__((packed)) navigation_t;


extern navigation_t navigation;



#endif
