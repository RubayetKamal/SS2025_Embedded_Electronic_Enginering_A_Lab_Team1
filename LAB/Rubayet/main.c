#include <stdio.h>
#include "edf.h"
#include <stdlib.h>

int timer = 0;

int main()
{
	task *arrayOfTasksForUser;
	int numberOfTasksForUser, hyper_Period, active_Task_ID;
	float SchedulabilityOfTasksForUser;

	printf("Enter number of tasks\n");
	scanf("%d", &numberOfTasksForUser);

	arrayOfTasksForUser = malloc(numberOfTasksForUser * sizeof(task));

	Retrieve_AND_Initialise_Tasks(arrayOfTasksForUser, numberOfTasksForUser); //get all task parameters and initialise absolute values

	SchedulabilityOfTasksForUser = Test_Schedulability(arrayOfTasksForUser, numberOfTasksForUser); //formula for schedulability test
	printf("CPU Utilization %f\n", SchedulabilityOfTasksForUser);
	if (SchedulabilityOfTasksForUser < 1)
		printf("Tasks can be scheduled\n");
	else
		printf("Schedule is not feasible\n");

	hyper_Period = Calculate_Hyperperiod(arrayOfTasksForUser, numberOfTasksForUser);
	Copy_Execution_Time(arrayOfTasksForUser, numberOfTasksForUser, ALL);
	Update_Absolute_arrivalTime(arrayOfTasksForUser, numberOfTasksForUser, 0, ALL);
	Update_Absolute_deadlineTime(arrayOfTasksForUser, numberOfTasksForUser, ALL);

    printPeriodsOfTasks(arrayOfTasksForUser, numberOfTasksForUser);

	while (timer <= hyper_Period)
	{

		if (InterruptForArrivalOfNewTask(arrayOfTasksForUser, timer, numberOfTasksForUser)) //Is it time to consider a new task?
		{
			active_Task_ID = findTaskIDwithLowestDeadline(arrayOfTasksForUser, numberOfTasksForUser, abs_deadline); //update the currently active task, choosing the task with the earliest absolute deadline
		}

		if (active_Task_ID == IDLE_TASK_ID) //Means nothing is running right now
		{
			printf("%d  Idle\n", timer);
		}

		if (active_Task_ID != IDLE_TASK_ID) //There is active task
		{

			if (arrayOfTasksForUser[active_Task_ID].TaskAttributes[execution_copy] != 0) //Task has not finished
			{
				arrayOfTasksForUser[active_Task_ID].TaskAttributes[execution_copy]--; //run it for one time unit
				printf("%d  Task %d\n", timer, active_Task_ID + 1); //
			}

			if (arrayOfTasksForUser[active_Task_ID].TaskAttributes[execution_copy] == 0) //the task has finished 
			{
				arrayOfTasksForUser[active_Task_ID].instance++; //for the next job
				arrayOfTasksForUser[active_Task_ID].alive = 0; //set as not alive
				Copy_Execution_Time(arrayOfTasksForUser, active_Task_ID, CURRENT); //Reset execution copy
				Update_Absolute_arrivalTime(arrayOfTasksForUser, active_Task_ID, arrayOfTasksForUser[active_Task_ID].instance, CURRENT);//computing new arrival
				Update_Absolute_deadlineTime(arrayOfTasksForUser, active_Task_ID, CURRENT); //computing new deadline
				active_Task_ID = findTaskIDwithLowestDeadline(arrayOfTasksForUser, numberOfTasksForUser, abs_deadline); //selecting new task with earliest deadline
			}
		}
		++timer; //timer moves forward
	}
	free(arrayOfTasksForUser);
	return 0;
}
