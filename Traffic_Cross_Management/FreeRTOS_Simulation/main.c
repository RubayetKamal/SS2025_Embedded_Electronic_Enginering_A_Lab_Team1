#include <stdio.h>
#include "pedestrianLights.h"
#include "pedestrian.h"
#include "cars.h"
#include "trafficLights.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdlib.h>
#include "FreeRTOSConfig.h"
#include "hooks.h"

extern QueueHandle_t northQueue;
extern QueueHandle_t southQueue;
extern QueueHandle_t eastQueue;
extern QueueHandle_t westQueue;



int main() {
    initTrafficLights();
    initPedestrianLights();

    northQueue = xQueueCreate(10, sizeof(Car));
    southQueue = xQueueCreate(10, sizeof(Car));
    eastQueue  = xQueueCreate(10, sizeof(Car));
    westQueue  = xQueueCreate(10, sizeof(Car));

    xTaskCreate(intersectionManager, "Intersection", 1024, NULL, 2, NULL);

    xTaskCreate(carGeneratorTask, "CarGen", 1024, NULL, 1, NULL);
    xTaskCreate(pedestrianGeneratorTask, "PedGen", 512, NULL, 1, NULL);

    static Direction d1 = NORTH, d2 = SOUTH, d3 = EAST, d4 = WEST;
    xTaskCreate(carQueueProcessorTask, "CarNorth", 1024, &d1, 1, NULL);
    xTaskCreate(carQueueProcessorTask, "CarSouth", 1024, &d2, 1, NULL);
    xTaskCreate(carQueueProcessorTask, "CarEast",  1024, &d3, 1, NULL);
    xTaskCreate(carQueueProcessorTask, "CarWest",  1024, &d4, 1, NULL);

    vTaskStartScheduler();
}
