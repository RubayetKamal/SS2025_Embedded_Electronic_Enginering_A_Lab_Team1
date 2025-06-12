#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vApplicationMallocFailedHook(void) {
    fprintf(stderr, "Malloc failed!\n");
    while (1);  // Loop forever for debug
}

// Called when a task overflows its stack
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    fprintf(stderr, "Stack overflow in task: %s\n", pcTaskName);
    while (1);  // Loop forever for debug
}
