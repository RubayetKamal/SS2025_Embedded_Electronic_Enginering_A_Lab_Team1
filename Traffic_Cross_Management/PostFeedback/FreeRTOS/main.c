#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hooks.h"
#include "portable.h"
#include "FreeRTOSConfig.h"

#define MAX_CARS 10



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
} Car;

int computePriority(Car* car);
bool canProceed(Car* self);

Car* carQueue[MAX_CARS] = { NULL };
SemaphoreHandle_t carQueueMutex;

static const char* dirNames[] = {"NORTH", "EAST", "SOUTH", "WEST"};
static const char* turnNames[] = {"LEFT", "STRAIGHT", "RIGHT"};

int computePriority(Car* car) {
    int base = 0;
    switch (car->turn) {
        case STRAIGHT: base = 3; break;
        case RIGHT:    base = 2; break;
        case LEFT:     base = 1; break;
    }
    return base * 10 + (3 - car->from); 
}

bool canProceed(Car* self) {
    for (int i = 0; i < MAX_CARS; i++) {
        Car* other = carQueue[i];
        if (other && other->active && other->id != self->id) {
            if (computePriority(other) > computePriority(self)) {
                return false;
            }
        }
    }
    return true;
}

void carTask(void *params) {
    Car* car = (Car*)params;

    car->arrivalTime = xTaskGetTickCount();
    car->active = true;

    xSemaphoreTake(carQueueMutex, portMAX_DELAY);
    for (int i = 0; i < MAX_CARS; i++) {
        if (carQueue[i] == NULL) {
            carQueue[i] = car;
            break;
        }
    }
    xSemaphoreGive(carQueueMutex);

    printf("🚘 Car %d arrived from %s going %s\n", car->id, dirNames[car->from], turnNames[car->turn]);

    while (1) {
        xSemaphoreTake(carQueueMutex, portMAX_DELAY);
        bool canGo = canProceed(car);
        xSemaphoreGive(carQueueMutex);

        if (canGo) {
            TickType_t now = xTaskGetTickCount();
            TickType_t waitTime = now - car->arrivalTime;
            printf("✅ Car %d ENTERED from %s going %s after %lu ms\n",
                   car->id, dirNames[car->from], turnNames[car->turn], waitTime * portTICK_PERIOD_MS);

            vTaskDelay(pdMS_TO_TICKS(2000)); 

            xSemaphoreTake(carQueueMutex, portMAX_DELAY);
            car->active = false;
            for (int i = 0; i < MAX_CARS; i++) {
                if (carQueue[i] == car) {
                    carQueue[i] = NULL;
                    break;
                }
            }
            xSemaphoreGive(carQueueMutex);

            printf("🏁 Car %d EXITED\n", car->id);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    vPortFree(car);
    vTaskDelete(NULL);
}

void carGeneratorTask(void *params) {
    int id = 1;
    while (1) {
        Car* car = (Car*) pvPortMalloc(sizeof(Car)); 
        if (car == NULL) {
            printf("🚨 malloc failed for Car %d!\n", id);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        car->id = id++;
        car->from = rand() % 4;
        car->turn = rand() % 3;
        car->active = false;

        BaseType_t taskCreated = xTaskCreate(carTask, "Car", 1024, car, 1, NULL);
        if (taskCreated != pdPASS) {
            printf("🚨 xTaskCreate failed for Car %d! Freeing memory.\n", car->id);
            vPortFree(car);  
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



int main() {
    carQueueMutex = xSemaphoreCreateMutex();

    xTaskCreate(carGeneratorTask, "CarGen", 1024, NULL, 1, NULL);

    vTaskStartScheduler();
}
