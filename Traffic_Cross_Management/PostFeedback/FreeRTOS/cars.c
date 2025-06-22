#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "cars.h"

#define MAX_CARS        10
#define CROSS_TIME_MS   2000
#define SPAWN_INTERVAL_MS 1000

SemaphoreHandle_t carQueueMutex   = NULL;
SemaphoreHandle_t logMutex = NULL;

static const char* dirNames[]  = { "NORTH", "EAST", "SOUTH", "WEST" };
static const char* turnNames[] = { "LEFT", "STRAIGHT", "RIGHT" };

static QueueHandle_t    dirQueue[4];
static SemaphoreHandle_t arbiterMutex;

void    arbiterTask(void *pv);
void    carTask(void *pv);
void    carGeneratorTask(void *pv);
void           initFCFS(void);

// Call this once before spawning any car tasks
void initFCFS(void) {
    for (int d = 0; d < 4; ++d) {
        dirQueue[d] = xQueueCreate(MAX_CARS, sizeof(Car*));
    }
    arbiterMutex = xSemaphoreCreateMutex();
    logMutex = xSemaphoreCreateMutex();  // Initialize log mutex
    srand(time(NULL));  // Seed random generator
    
    xTaskCreate(arbiterTask, "Arbiter", 1024, NULL,
                tskIDLE_PRIORITY + 2, NULL);
}

// Picks the head‐of‐line car with the earliest arrivalTime
void arbiterTask(void *pv) {
    for (;;) {
        Car *bestCar = NULL, *peeked = NULL;
        int  bestDir = -1;

        xSemaphoreTake(arbiterMutex, portMAX_DELAY);
        for (int d = 0; d < 4; ++d) {
            if (uxQueueMessagesWaiting(dirQueue[d]) == 0) continue;
            xQueuePeek(dirQueue[d], &peeked, 0);
            if (!bestCar || peeked->arrivalTime < bestCar->arrivalTime) {
                bestCar = peeked;
                bestDir = d;
            }
        }
        if (bestCar) {
            xQueueReceive(dirQueue[bestDir], &bestCar, 0);
            xSemaphoreGive(bestCar->goSem);
        }
        xSemaphoreGive(arbiterMutex);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Thread-safe logging function
void log_event(const char* event, Car* car) {
    xSemaphoreTake(logMutex, portMAX_DELAY);
    
    FILE *logFile = fopen("C:\\Users\\User\\PyCharmMiscProject\\intersection_log.txt", "a");
    if (logFile) {
        fprintf(logFile, "%lu,%s,%d,%s,%s\n",
                xTaskGetTickCount(), event, car->id, 
                dirNames[car->from], turnNames[car->turn]);
        fclose(logFile);
    }
    
    xSemaphoreGive(logMutex);
}

// Each car enqueues itself, waits for the arbiter, then crosses
void carTask(void *pv) {
    Car *car = (Car*)pv;
    car->arrivalTime = xTaskGetTickCount();
    car->active      = true;
    car->goSem       = xSemaphoreCreateBinary();

    log_event("ARRIVAL", car);
    printf("Car %d arrived from %s going %s\n",
           car->id, dirNames[car->from], turnNames[car->turn]);
    vTaskDelay(pdMS_TO_TICKS(10000));

    xQueueSend(dirQueue[car->from], &car, portMAX_DELAY);
    xSemaphoreTake(car->goSem, portMAX_DELAY);

    log_event("ENTER", car);
    printf("Car %d ENTERED after %lu ms\n", car->id,
           (xTaskGetTickCount() - car->arrivalTime) * portTICK_PERIOD_MS);

    vTaskDelay(pdMS_TO_TICKS(10000));

    log_event("EXIT", car);
    printf("Car %d EXITED\n", car->id);
    vTaskDelay(pdMS_TO_TICKS(3000));

    vSemaphoreDelete(car->goSem);
    vPortFree(car);
    vTaskDelete(NULL);
}

// Spawns cars at fixed intervals
void carGeneratorTask(void *pv) {
    int id = 1;
    for (;;) {
        Car *car = pvPortMalloc(sizeof *car);
        if (!car) {
            printf("malloc failed for Car %d\n", id);
            vTaskDelay(pdMS_TO_TICKS(SPAWN_INTERVAL_MS));
            continue;
        }

        car->id   = id++;
        car->from = rand() % 4;
        car->turn = rand() % 3;
        car->active = false;

        if (xTaskCreate(carTask, "Car", 1024, car,
                        tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        {
            printf("xTaskCreate failed for Car %d\n", car->id);
            vPortFree(car);
        }

        vTaskDelay(pdMS_TO_TICKS(SPAWN_INTERVAL_MS));
    }
}
