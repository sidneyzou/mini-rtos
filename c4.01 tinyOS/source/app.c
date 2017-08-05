#include "tinyOs.h"


int task1flag;
int task2flag;
int task3flag;
tTask tTask1;
tTask tTask2;
tTask tTask3;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];

void task1Entry (void *param)
{ 
    tSetSysTickPeriod(10);
    for (;;) {
        
        task1flag = 0;
        tTaskSuspend(currentTask);
        task1flag = 1;
        tTaskSuspend(currentTask);
    }
}


void task2Entry (void *param)
{
    for (;;) {
        task2flag = 0;
        tTaskDelay(1);
        tTaskWakeUp(&tTask1);
        task2flag = 1;
        tTaskDelay(1);
        tTaskWakeUp(&tTask1);
    }
}

void task3Entry (void *param)
{
    for (;;) {
        task3flag = 0;
        tTaskDelay(1);
        task3flag = 1;
        tTaskDelay(1);
    }
}

void tInitApp(void)
{
    tTaskInit (&tTask1, task1Entry, (void *)(0x11111111), 0, &task1Env[1024]);
    tTaskInit (&tTask2, task2Entry, (void *)(0x22222222), 1, &task2Env[1024]);
    tTaskInit (&tTask3, task3Entry, (void *)(0x22222222), 1, &task3Env[1024]);
}
