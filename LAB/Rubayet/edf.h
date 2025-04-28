#ifndef EDF_H
#define EDF_H

#define arrival			0 //The attributes of a task are defined 
#define execution 		1
#define deadline		2
#define period			3
#define abs_arrival		4 
#define execution_copy  5
#define abs_deadline	6

//stucture of a task
typedef struct
{
	int TaskAttributes[7],instance,alive;
}task;

#define IDLE_TASK_ID 1023   // Just a number I assigned
#define ALL 1 				//Flag for all tasks
#define CURRENT 0			//Flag for just current task

void Retrieve_AND_Initialise_Tasks(task *pointerToFirstTask,int numberOfTasks);				//Get tasks parameters - Arrival time , Execution time , Deasdline and Period
void InitialiseTasksOtherAttributes(task *pointerToFirstTask);
int Calculate_Hyperperiod(task *pointerToFirstTask,int numberOfTasks);			//Calculate hyperiod period of the task set  
float Test_Schedulability(task *pointerToFirstTask,int numberOfTasks);				//Calculates CPU utilization
int Get_GreatestCommonDivisor(int resultOfLCM, int periodStored);					//Find greatest common divisor
int Get_LeastCommonMultiple(int *periodStorage, int numberOfTasks);					//Find Least common multiple
int InterruptForArrivalOfNewTask(task *pointerToFirstTask,int currentTime,int numberOfTasks);		//Scheduling point interrupt
int findTaskIDwithLowestDeadline(task *pointerToFirstTask,int numberOfTasks,int Task_Period);				//Find minimum of given task parameter
void Update_Absolute_arrivalTime(task *pointerToFirstTask,int numberOfTasks,int instanceNumber,int allTasksFlag);	//Update absolute arrival time (absolute arrival time = arrivaltime + instance*period) 
void Update_Absolute_deadlineTime(task *pointerToFirstTask,int numberOfTasks,int allTasksFlag);	//Update absolute deadline (absolute deadline = absolute arrival time + deadline )
void Copy_Execution_Time(task *pointerToFirstTask,int numberOfTasks,int allTasksFlag);	//Keep a backup copy of execution time
void printPeriodsOfTasks(task *pointerToFirstTask, int numberOfTasks);
#endif