#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <timers.h>
#include <time.h>

volatile int i = 0;
#define T1_WCET 2 
#define T2_WCET 1 
#define T3_WCET 1
#define T4_WCET 2

void vTask1( void *pvParameters){

   while(1){ 
    i++;
    clock_t start=clock(); 
    for(int i = 0; i < 100000; i++);
    
    clock_t end=clock(); 
    double calk_time = (double) ( end - start ) * 1000/CLOCKS_PER_SEC;

    if (calk_time > T1_WCET ){

        printf("Deadlin Missed!!");

    } 
    else{
        printf("\n TASK 1 ");
    }
    printf("%d",i);
    vTaskDelay(pdMS_TO_TICKS(6000));

    } 

}

void vTask2( void *pvParameters){

    while(1){ 
        i++;
     clock_t start=clock(); 
     for(int i = 0; i < 100000; i++);
     
     clock_t end=clock(); 
     double calk_time = (double) ( end - start ) * 1000/CLOCKS_PER_SEC;
 
     if (calk_time > T2_WCET ){
 
         printf("Deadlin Missed!!");
 
     } 
     else{
         printf("\n Task 2 ");
     }
     printf("%d",i);
     vTaskDelay(pdMS_TO_TICKS(3000));
 
     } 
 
 }

 void vTask3( void *pvParameters){

    while(1){ 
        i++;
     clock_t start=clock(); 
     for(int i = 0; i < 100000; i++);
     
     clock_t end=clock(); 
     double calk_time = (double) ( end - start ) * 1000/CLOCKS_PER_SEC;
 
     if (calk_time > T3_WCET ){
 
         printf("Deadlin Missed!!");
 
     } 
     else{
         printf("\n TAsk 3");
     }
     printf("%d",i);
     vTaskDelay(pdMS_TO_TICKS(7000));
 
     } 
 
 }

 void vTask4( void *pvParameters){

    while(1){ 
        i++;
     clock_t start=clock(); 
     for(int i = 0; i < 100000; i++);
     
     clock_t end=clock(); 
     double calk_time = (double) ( end - start ) * 1000/CLOCKS_PER_SEC;
 
     if (calk_time > T4_WCET ){
 
         printf("Deadlin Missed!!");
 
     } 
     else{
         printf("\n Task 4 ");
     }
     printf("%d",i);
     vTaskDelay(pdMS_TO_TICKS(9000));
 
     } 
 
 }

int main (){

xTaskCreate(vTask1, "TASK1", 2048, NULL, 1, NULL);
xTaskCreate(vTask2, "TASK2", 2048, NULL, 1, NULL);
xTaskCreate(vTask3, "TASK3", 2048, NULL, 1, NULL);
xTaskCreate(vTask4, "TASK4", 2048, NULL, 1, NULL);
vTaskStartScheduler();

}

