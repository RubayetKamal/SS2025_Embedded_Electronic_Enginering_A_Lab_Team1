#ifndef CARS_H
#define CARS_H

#include "portable.h"
#include "semaphore.h"

#define MAX_CARS 10
SemaphoreHandle_t carQueueMutex;

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST
} Direction;

typedef enum {
    LEFT,
    STRAIGHT,
    RIGHT
} TurnDirection;

typedef struct {
    int id;
    Direction from;
    TurnDirection turn;
    TickType_t arrivalTime;
    bool active;
} Car;

int computePriority(Car* car);
bool canProceed(Car* self);
void carTask(void *params);
void carGeneratorTask(void *params);

#endif
