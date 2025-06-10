#include "pedestrianLights.h"     // Needed for pedestrian light control
#include "pedestrian.h"
#include <stdio.h>          // Needed for printf
#include "FreeRTOS.h"  // FreeRTOS headers
#include "task.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
#include <stdbool.h>

extern PedestrianLight pedLightNorth1;
extern PedestrianLight pedLightSouth1;
extern PedestrianLight pedLightEast1;
extern PedestrianLight pedLightWest1;

QueueHandle_t pedQueueNorth, pedQueueSouth, pedQueueEast, pedQueueWest;
int pedestrianId = 1;
bool pedestrianCrossingActive[8] = {false};

void pedestrianGeneratorTask(void *params)
{
    while (1)
    {
        PedestrianCrossing crossing = rand() % 8;

        if (pedestrianLights[crossing].state == PED_GREEN && !pedestrianCrossingActive[crossing])
        {
            pedestrianCrossingActive[crossing] = true;

            printf("🚶 Pedestrian %d started crossing at %s\n", pedestrianId++, pedestrianLightNames[crossing]);

            vTaskDelay(pdMS_TO_TICKS(3000)); // Simulate 3s crossing time

            pedestrianCrossingActive[crossing] = false;
            printf("✅ Pedestrian finished crossing at %s\n", pedestrianLightNames[crossing]);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1s before next pedestrian
    }
}

