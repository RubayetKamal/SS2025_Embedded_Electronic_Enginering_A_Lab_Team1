#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H


#include <stdbool.h>
#include "trafficLights.h"

extern bool pedestrianCrossingActive[8];

typedef struct {
    Direction direction;
    int id;
} Pedestrian;




void pedestrianGeneratorTask(void *params);

#endif