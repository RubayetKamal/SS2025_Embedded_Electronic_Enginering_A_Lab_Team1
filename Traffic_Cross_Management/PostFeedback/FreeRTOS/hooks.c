#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vApplicationMallocFailedHook(void) {
    printf("🔥 FreeRTOS malloc failed! Heap is exhausted.\n");
    taskDISABLE_INTERRUPTS();
    for (;;); // or reset MCU
}

// Called when a task overflows its stack
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    fprintf(stderr, "Stack overflow in task: %s\n", pcTaskName);
    while (1);  // Loop forever for debug
}
