#include "FreeRTOS.h"  // FreeRTOS headers
#include "task.h"
#include "queue.h"
#include "cars.h"
#include "trafficLights.h"
#include "pedestrian.h"
#include "pedestrianLights.h"
#include "FreeRTOSConfig.h"
#include <stdbool.h>

QueueHandle_t northQueue, southQueue, eastQueue, westQueue;

const char* movementToStr(Movement mv) {
    switch (mv) {
        case GO_STRAIGHT: return "GO_STRAIGHT";
        case TURN_LEFT:   return "TURN_LEFT";
        case TURN_RIGHT:  return "TURN_RIGHT";
        default:          return "UNKNOWN";
    }
}

bool isPedestrianBlocking(Direction carDirection)
{
    // Match direction with corresponding pedestrian crossings
    switch (carDirection)
    {
        case NORTH:
            return pedestrianCrossingActive[NorthToSouth1] || pedestrianCrossingActive[NorthToSouth2];
        case SOUTH:
            return pedestrianCrossingActive[SouthToNorth1] || pedestrianCrossingActive[SouthToNorth2];
        case EAST:
            return pedestrianCrossingActive[EastToWest1] || pedestrianCrossingActive[EastToWest2];
        case WEST:
            return pedestrianCrossingActive[WestToEast1] || pedestrianCrossingActive[WestToEast2];
        default:
            return false;
    }
}



void carGeneratorTask(void *params) {
    int carID = 1;

    while (1) {
        Car newCar;
        newCar.origin = rand() % 4; // NORTH, SOUTH, EAST, WEST
        newCar.action = rand() % 3; // GO_STRAIGHT, TURN_LEFT, TURN_RIGHT
        newCar.carID = carID++;

        QueueHandle_t q;
        switch (newCar.origin) {
            case NORTH: q = northQueue; break;
            case SOUTH: q = southQueue; break;
            case EAST:  q = eastQueue;  break;
            case WEST:  q = westQueue;  break;
        }

        if (xQueueSend(q, &newCar, 0) == pdPASS) {
           printf("🚗 Car %d added to queue from %s going to %s\n", newCar.carID, directionToStr(newCar.origin), movementToStr(newCar.action));
        }else{
            printf("❌ Queue %d full. Car %d dropped.\n", newCar.origin, newCar.carID);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // new car every second
    }
}

void carQueueProcessorTask(void *params) {
    Direction dir = *(Direction *)params;
    QueueHandle_t q;

    switch (dir) {
        case NORTH: q = northQueue; break;
        case SOUTH: q = southQueue; break;
        case EAST:  q = eastQueue;  break;
        case WEST:  q = westQueue;  break;
    }

    while (1) {
        Car car;
        if (xQueuePeek(q, &car, portMAX_DELAY) == pdPASS) {
            // Get light for this direction
            TrafficLight *light;
            switch (dir) {
                case NORTH: light = &northLight; break;
                case SOUTH: light = &southLight; break;
                case EAST:  light = &eastLight;  break;
                case WEST:  light = &westLight;  break;
            }

            // ✅ Check if light is green and no pedestrian is crossing
            if (light->state == GREEN && !isPedestrianBlocking(dir)) {
                xQueueReceive(q, &car, 0); // Dequeue
                printf("✅ Car %d from %s going to %s is MOVING\n", car.carID, directionToStr(car.origin), movementToStr(car.action));
                vTaskDelay(pdMS_TO_TICKS(1000)); // simulate movement
            } else {
                if (light->state != GREEN) {
                    printf("⛔ Car %d waiting: RED light at %s\n", car.carID, directionToStr(car.origin));
                } else if (isPedestrianBlocking(dir)) {
                    printf("🚶 Car %d waiting: pedestrian crossing at %s\n", car.carID, directionToStr(car.origin));
                }
                vTaskDelay(pdMS_TO_TICKS(200)); // wait and re-check
            }
        }
    }
}
