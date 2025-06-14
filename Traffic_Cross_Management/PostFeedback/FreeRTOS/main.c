#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hooks.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "cars.h"
#include "pedestrian.h"


int main() {
    carQueueMutex = xSemaphoreCreateMutex();
    crossingMutex = xSemaphoreCreateMutex();

    xTaskCreate(carGeneratorTask, "CarGen", 1024, NULL, 1, NULL);
    xTaskCreate(pedestrianTask, "Pedestrian", 1024, NULL, 1, NULL);

    vTaskStartScheduler();
}
