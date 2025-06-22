#ifndef CARS_H
#define CARS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "portable.h"
#include "semphr.h"

#define MAX_CARS 10
extern SemaphoreHandle_t carQueueMutex;
extern SemaphoreHandle_t logMutex;
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
    SemaphoreHandle_t goSem;
} Car;

void initFCFS(void);
void arbiterTask(void *pv);
void carTask(void *pv);
void carGeneratorTask(void *pv);
void log_event(const char* event, Car* car);

#endif
