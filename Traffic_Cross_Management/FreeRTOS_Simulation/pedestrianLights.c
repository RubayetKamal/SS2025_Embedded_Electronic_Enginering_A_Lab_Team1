#include "pedestrianLights.h"
#include "FreeRTOS.h"  // FreeRTOS headers
#include "task.h"
#include <stdio.h>
#include "queue.h"
#include "FreeRTOSConfig.h"

PedestrianLight pedestrianLights[8];
const char* pedestrianLightNames[] = {
    "PedestrianLightFacingNorth1",
    "PedestrianLightFacingNorth2",
    "PedestrianLightFacingSouth1",
    "PedestrianLightFacingSouth2",
    "PedestrianLightFacingEast1",
    "PedestrianLightFacingEast2",
    "PedestrianLightFacingWest1",
    "PedestrianLightFacingWest2"
};

void setPedestrianLight(PedestrianCrossing crossing, PedestrianState state)
{
    pedestrianLights[crossing].state = state;
    printf("%s set to %s\n", pedestrianLightNames[crossing], state == PED_GREEN ? "GREEN" : "RED");
}

void initPedestrianLights()
{
    for (int i = 0; i < 8; i++)
    {
        pedestrianLights[i].crossing = (PedestrianCrossing)i;
        pedestrianLights[i].state = PED_RED;
    }
}