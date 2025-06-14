#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hooks.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "pedestrian.h"
#include "cars.h"


volatile PedestrianCrossing activeCrossing = CROSS_NONE;
SemaphoreHandle_t crossingMutex;

const char* movementToStr(PedestrianCrossing pedCross){
    switch (pedCross) {
        case CROSS_N_S_W: return "On West Crossing from North to South";
        case CROSS_S_N_W:   return "On West Crossing from South to North";
        case CROSS_N_S_E:  return "On East Crossing From North to South ";
        case CROSS_S_N_E: return "On East Crossing From South to North ";
        case CROSS_W_E_N:   return "On North Crossing From West to East ";
        case CROSS_E_W_N:  return "On North Crossing From East to West ";
        case CROSS_W_E_S: return "On South Crossing From West to East ";
        case CROSS_E_W_S:   return "On South Crossing From East to West ";
        default:          return "UNKNOWN";
    }
}


void pedestrianTask(void *params) {
    while (1) {
        int wait = (rand() % 10 + 5) * 1000;
        vTaskDelay(pdMS_TO_TICKS(wait));

        PedestrianCrossing chosen = rand() % 8;

        xSemaphoreTake(crossingMutex, portMAX_DELAY);
        activeCrossing = chosen;
        xSemaphoreGive(crossingMutex);

        printf("Pedestrian %s\n", movementToStr(chosen));

        vTaskDelay(pdMS_TO_TICKS(3000)); // Pedestrian crossing time
        vTaskDelay(pdMS_TO_TICKS(1000)); // Small safety delay

        xSemaphoreTake(crossingMutex, portMAX_DELAY);
        activeCrossing = CROSS_NONE;
        xSemaphoreGive(crossingMutex);

        printf("Pedestrian crossing cleared\n");
    }
}


bool isBlockedByPedestrian(Car* car) {
    bool blocked = false;

    xSemaphoreTake(crossingMutex, portMAX_DELAY);
    PedestrianCrossing crossing = activeCrossing;
    xSemaphoreGive(crossingMutex);

    switch (crossing) {
        case CROSS_N_S_W:
            if (car->from == WEST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == LEFT)) blocked = true;
            if (car->from == NORTH && (car->turn == RIGHT)) blocked = true;
            break;

        case CROSS_S_N_W:
            if (car->from == WEST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == LEFT)) blocked = true;
            if (car->from == NORTH && (car->turn == RIGHT)) blocked = true;
            break;

        case CROSS_N_S_E:
            if (car->from == WEST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == RIGHT)) blocked = true;
            if (car->from == NORTH && (car->turn == LEFT)) blocked = true;
            break;

        case CROSS_S_N_E:
            if (car->from == WEST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == RIGHT)) blocked = true;
            if (car->from == NORTH && (car->turn == LEFT)) blocked = true;
            break;      

        case CROSS_W_E_N:
            if (car->from == NORTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == WEST && (car->turn == LEFT)) blocked = true;
            if (car->from == EAST && (car->turn == RIGHT)) blocked = true;
            break;

        case CROSS_E_W_N:
            if (car->from == NORTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == WEST && (car->turn == LEFT)) blocked = true;
            if (car->from == EAST && (car->turn == RIGHT)) blocked = true;
            break;            

        case CROSS_W_E_S:
            if (car->from == NORTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == WEST && (car->turn == RIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == LEFT)) blocked = true;
            break;

        case CROSS_E_W_S:
            if (car->from == NORTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == SOUTH && (car->turn == STRAIGHT)) blocked = true;
            if (car->from == WEST && (car->turn == RIGHT)) blocked = true;
            if (car->from == EAST && (car->turn == LEFT)) blocked = true;
            break;            

        default:
            break;
    }

    return blocked;
}
