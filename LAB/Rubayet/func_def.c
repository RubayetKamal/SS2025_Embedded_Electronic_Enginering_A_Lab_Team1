#include "edf.h"
#include <stdio.h>

void Retrieve_AND_Initialise_Tasks(task *pointerToFirstTask,int numberOfTasks)
{
	for (int i=0;i<numberOfTasks;i++)
	{
		printf("Enter Task %d parameters\n", i + 1);
		printf("Arrival time: ");
		scanf("%d", &pointerToFirstTask->TaskAttributes[arrival]); //for scanf memory address is used
		printf("Execution time: ");
		scanf("%d", &pointerToFirstTask->TaskAttributes[execution]);
		printf("Deadline time: ");
		scanf("%d", &pointerToFirstTask->TaskAttributes[deadline]);
		printf("Period: ");
		scanf("%d", &pointerToFirstTask->TaskAttributes[period]);

        InitialiseTasksOtherAttributes(pointerToFirstTask);
		pointerToFirstTask++; //points to the next task
	}
}

void InitialiseTasksOtherAttributes(task *pointerToFirstTask){
    pointerToFirstTask->TaskAttributes[abs_arrival] = 0;
    pointerToFirstTask->TaskAttributes[execution_copy] = 0;
    pointerToFirstTask->TaskAttributes[abs_deadline] = 0;
    pointerToFirstTask->instance = 0;
    pointerToFirstTask->alive = 0;
}

void printPeriodsOfTasks(task *pointerToFirstTask, int numberOfTasks)
{
	printf("Periods:");
	for (int i = 0; i < numberOfTasks; i++) {
    printf(" Task %d=%d", i + 1, pointerToFirstTask[i].TaskAttributes[period]);
	}
	printf("\n");
}

float Test_Schedulability(task *pointerToFirstTask,int numberOfTasks){
    float utilization = 0;
    for(int i =0; i<numberOfTasks;i++){
        utilization += ((float)(pointerToFirstTask->TaskAttributes[execution])) / ((float)(pointerToFirstTask->TaskAttributes[deadline]));
        pointerToFirstTask++;
    }
    return utilization;
}

int Calculate_Hyperperiod(task *pointerToFirstTask,int numberOfTasks)
{
	int hyperPeriod, periodStorage[10];
	for(int i=0;i<numberOfTasks;i++)
	{
		periodStorage[i] = pointerToFirstTask->TaskAttributes[period];
		pointerToFirstTask++;
	}
    
	hyperPeriod = Get_LeastCommonMultiple(periodStorage, numberOfTasks);

	return hyperPeriod;
}

int Get_GreatestCommonDivisor(int resultOfLCM, int periodStored)
{
	if (periodStored == 0)
		return resultOfLCM;
	else
		return Get_GreatestCommonDivisor(periodStored, resultOfLCM % periodStored);
}

int Get_LeastCommonMultiple(int *periodStorage, int numberOfTasks)
{
	int resultOfLCM = 1;
	for (int i = 0; i < numberOfTasks; i++)
	{
		resultOfLCM = (resultOfLCM * periodStorage[i]) / Get_GreatestCommonDivisor(resultOfLCM, periodStorage[i]);
	}
	return resultOfLCM;
}

int InterruptForArrivalOfNewTask(task *pointerToFirstTask,int currentTime,int numberOfTasks)
{
	int unaliveTasksCount = 0, arrivingTasksCount = 0; 
	task *copy_pointerToFirstTask;	//it saves the original pointer location so that it can be reset later
	copy_pointerToFirstTask = pointerToFirstTask;
    for(int i=0; i< numberOfTasks;i++)
	{
		if (currentTime == pointerToFirstTask->TaskAttributes[abs_arrival]) 
		{
			pointerToFirstTask->alive = 1;
			arrivingTasksCount++;
		}
		pointerToFirstTask++;
	}

	pointerToFirstTask = copy_pointerToFirstTask;

    for(int i=0;i<numberOfTasks;i++)
	{
		if (pointerToFirstTask->alive == 0)
			unaliveTasksCount++;
		    pointerToFirstTask++;
	}

	if (unaliveTasksCount == numberOfTasks || arrivingTasksCount != 0)
	{
		return 1;
	}

	return 0; 
}

void Update_Absolute_deadlineTime(task *pointerToFirstTask,int numberOfTasks,int allTasksFlag)
{
	
	if (allTasksFlag)
	{
		for (int i = 0;i<numberOfTasks;i++)
		{
			pointerToFirstTask->TaskAttributes[abs_deadline] = pointerToFirstTask->TaskAttributes[deadline] + pointerToFirstTask->TaskAttributes[abs_arrival];
			pointerToFirstTask++;
		}
	}
	else
	{
		pointerToFirstTask += numberOfTasks;
		pointerToFirstTask->TaskAttributes[abs_deadline] = pointerToFirstTask->TaskAttributes[deadline] + pointerToFirstTask->TaskAttributes[abs_arrival];
	}

}

void Update_Absolute_arrivalTime(task *pointerToFirstTask,int numberOfTasks,int instanceNumber,int allTasksFlag)
{
	if (allTasksFlag)
	{
		for(int i = 0;i<numberOfTasks;i++)
		{
			pointerToFirstTask->TaskAttributes[abs_arrival] = pointerToFirstTask->TaskAttributes[arrival] + (instanceNumber * (pointerToFirstTask->TaskAttributes[period]));
			pointerToFirstTask++;
		}
	}
	else
	{
		pointerToFirstTask += numberOfTasks;
		pointerToFirstTask->TaskAttributes[abs_arrival] = pointerToFirstTask->TaskAttributes[arrival] + (instanceNumber * (pointerToFirstTask->TaskAttributes[period]));
	}
}

void Copy_Execution_Time(task *pointerToFirstTask,int numberOfTasks,int allTasksFlag)
{
	if (allTasksFlag)
	{
		for(int i = 0;i<numberOfTasks;i++)
		{
			pointerToFirstTask->TaskAttributes[execution_copy] = pointerToFirstTask->TaskAttributes[execution];
			pointerToFirstTask++;
		}
	}
	else
	{
		pointerToFirstTask += numberOfTasks;
		pointerToFirstTask->TaskAttributes[execution_copy] = pointerToFirstTask->TaskAttributes[execution];
	}
}

int findTaskIDwithLowestDeadline(task *pointerToFirstTask,int numberOfTasks,int absoluteDeadline)
{
	int dummy_minimum_value = 0x7FFF, task_id = IDLE_TASK_ID;
	for(int i = 0;i<numberOfTasks;i++)
	{
		if (dummy_minimum_value > pointerToFirstTask->TaskAttributes[absoluteDeadline] && pointerToFirstTask->alive == 1)
		{
			dummy_minimum_value = pointerToFirstTask->TaskAttributes[absoluteDeadline];
			task_id = i;
		}
		pointerToFirstTask++;
	}
	return task_id;
}