#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H

#include "semaphore.h"
#include "cars.h"


extern SemaphoreHandle_t crossingMutex;

typedef enum {
    CROSS_N_S_W, // North to South on West side
    CROSS_S_N_W,
    CROSS_N_S_E,
    CROSS_S_N_E,
    CROSS_W_E_N,
    CROSS_E_W_N,
    CROSS_W_E_S,
    CROSS_E_W_S,
    CROSS_NONE
} PedestrianCrossing;

void pedestrianTask(void *params);
bool isBlockedByPedestrian(Car* car);
const char* movementToStr(PedestrianCrossing pedCross);

#endif

