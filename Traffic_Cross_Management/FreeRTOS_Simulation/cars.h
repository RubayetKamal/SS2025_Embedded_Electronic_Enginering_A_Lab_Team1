#ifndef CARS_H
#define CARS_H

#include "trafficLights.h"

typedef enum {
    GO_STRAIGHT,
    TURN_LEFT,
    TURN_RIGHT
} Movement;

// typedef enum
// {
//     NORTH,
//     SOUTH,
//     EAST,
//     WEST
// } Direction;

typedef struct {
    Direction origin;
    Movement action;
    int carID;
} Car;


const char* movementToStr(Movement mv);

void carGeneratorTask(void *params);

void carQueueProcessorTask(void *params);

const char* directionToStr(Direction dir);

#endif