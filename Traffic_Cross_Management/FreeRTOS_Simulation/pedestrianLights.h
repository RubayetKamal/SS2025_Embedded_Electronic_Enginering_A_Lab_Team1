#ifndef PEDESTRIANLIGHTS_H
#define PEDESTRIANLIGHTS_H

typedef enum
{
    PED_RED,
    PED_GREEN
} PedestrianState;

typedef enum
{
    NorthToSouth1,
    NorthToSouth2,
    SouthToNorth1,
    SouthToNorth2,
    EastToWest1,
    EastToWest2,
    WestToEast1,
    WestToEast2
} PedestrianCrossing;

typedef struct
{
    PedestrianCrossing crossing;
    PedestrianState state;
} PedestrianLight;

extern PedestrianLight pedestrianLights[8];
extern const char* pedestrianLightNames[8];

void setPedestrianLight(PedestrianCrossing crossing, PedestrianState state);

void initPedestrianLights();

#endif