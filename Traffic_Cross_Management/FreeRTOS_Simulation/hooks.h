#ifndef HOOKS_H
#define HOOKS_H

#include "FreeRTOS.h"
#include "task.h"

// Called when FreeRTOS malloc fails
void vApplicationMallocFailedHook(void);

// Called when a task overflows its stack
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

#endif
