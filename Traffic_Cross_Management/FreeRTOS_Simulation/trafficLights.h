#ifndef TRAFFICLIGHTS_H
#define TRAFFICLIGHTS_H


typedef enum
{
    RED,
    RED_YELLOW,
    GREEN,
    YELLOW
} LightState;

typedef enum
{
    NORTH,
    SOUTH,
    EAST,
    WEST
} Direction;


typedef struct
{
    Direction dir;
    LightState state;
} TrafficLight;

extern TrafficLight northLight, southLight, eastLight, westLight;

const char* lightStateToStr(LightState state);

const char* directionToStr(Direction dir);

void initTrafficLights() ;

void setLightState(Direction dir, LightState state);

void intersectionManager(void *params);
#endif