#include "trafficLights.h"
#include "pedestrianLights.h"     // Needed for pedestrian light control
#include <stdio.h>          // Needed for printf
#include "FreeRTOS.h"  // FreeRTOS headers
#include "task.h"
#include "FreeRTOSConfig.h"

// Define global traffic lights (extern'd in .h)
TrafficLight northLight, southLight, eastLight, westLight;

const char* lightStateToStr(LightState state) {
    switch (state) {
        case RED:        return "RED";
        case RED_YELLOW: return "RED_YELLOW";
        case GREEN:      return "GREEN";
        case YELLOW:     return "YELLOW";
        default:         return "UNKNOWN";
    }
}

const char* directionToStr(Direction dir) {
    switch (dir) {
        case NORTH: return "NORTH";
        case SOUTH: return "SOUTH";
        case EAST:  return "EAST";
        case WEST:  return "WEST";
        default:    return "UNKNOWN";
    }
}

void initTrafficLights(void) {
    northLight.dir = NORTH;
    southLight.dir = SOUTH;
    eastLight.dir = EAST;
    westLight.dir = WEST;
    northLight.state = RED;
    southLight.state = RED;
    eastLight.state = RED;
    westLight.state = RED;
}

void setLightState(Direction dir, LightState state)
{
    const char* dirStr = directionToStr(dir);
    const char* stateStr = lightStateToStr(state);

    switch (dir)
    {
        case NORTH:
            northLight.state = state;
            break;
        case SOUTH:
            southLight.state = state;
            break;
        case EAST:
            eastLight.state = state;
            break;
        case WEST:
            westLight.state = state;
            break;
    }

    printf("🚦 Traffic light %s set to %s\n", dirStr, stateStr);
}

void intersectionManager(void *params)
{
    while (1)
    {
        // Phase 1: Prepare North-South
        setLightState(NORTH, RED_YELLOW);
        setLightState(SOUTH, RED_YELLOW);
        setLightState(EAST, RED);
        setLightState(WEST, RED);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Phase 2: North-South Green
        setLightState(NORTH, GREEN);
        setLightState(SOUTH, GREEN);
        setPedestrianLight(EastToWest1, PED_RED);
        setPedestrianLight(EastToWest2, PED_RED);
        setPedestrianLight(WestToEast1, PED_RED);
        setPedestrianLight(WestToEast2, PED_RED);
        setPedestrianLight(NorthToSouth1, PED_GREEN);
        setPedestrianLight(NorthToSouth2, PED_GREEN);
        setPedestrianLight(SouthToNorth1, PED_GREEN);
        setPedestrianLight(SouthToNorth2, PED_GREEN);
        vTaskDelay(pdMS_TO_TICKS(5000));

        // Phase 3: North-South Yellow
        setLightState(NORTH, YELLOW);
        setLightState(SOUTH, YELLOW);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Phase 4: All Red
        setLightState(NORTH, RED);
        setLightState(SOUTH, RED);
        setPedestrianLight(NorthToSouth1, PED_RED);
        setPedestrianLight(NorthToSouth2, PED_RED);
        setPedestrianLight(SouthToNorth1, PED_RED);
        setPedestrianLight(SouthToNorth2, PED_RED);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Phase 5: Prepare East-West
        setLightState(EAST, RED_YELLOW);
        setLightState(WEST, RED_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Phase 6: East-West Green
        setLightState(EAST, GREEN);
        setLightState(WEST, GREEN);
        setPedestrianLight(EastToWest1, PED_GREEN);
        setPedestrianLight(EastToWest2, PED_GREEN);
        setPedestrianLight(WestToEast1, PED_GREEN);
        setPedestrianLight(WestToEast2, PED_GREEN);
        setPedestrianLight(NorthToSouth1, PED_RED);
        setPedestrianLight(NorthToSouth2, PED_RED);
        setPedestrianLight(SouthToNorth1, PED_RED);
        setPedestrianLight(SouthToNorth2, PED_RED);
        vTaskDelay(pdMS_TO_TICKS(5000));

        // Phase 7: East-West Yellow
        setLightState(EAST, YELLOW);
        setLightState(WEST, YELLOW);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Phase 8: All Red
        setLightState(EAST, RED);
        setLightState(WEST, RED);
        setPedestrianLight(EastToWest1, PED_RED);
        setPedestrianLight(EastToWest2, PED_RED);
        setPedestrianLight(WestToEast1, PED_RED);
        setPedestrianLight(WestToEast2, PED_RED);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
