#include "tinyOs.h"


int task1flag;
int task2flag;
int task3flag;
int task4flag;
tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];
tTaskStack task4Env[1024];

tEvent eventWaitTimeout;
tEvent eventWaitNormal;
void task1Entry (void *param)
{ 
    tSetSysTickPeriod(10);
    
    tEventInit(&eventWaitTimeout, tEventTypeUnkonw);
    for (;;) {  
        tEventWait(&eventWaitTimeout, currentTask, 0, 0 ,5);
        tTaskSched();
        task1flag = 0;
        tTaskDelay(1);
        task1flag = 1;
        tTaskDelay(1);
    }
}


void task2Entry (void *param)
{
    for (;;) {
        tEventWait(&eventWaitNormal,currentTask,(void *)0, 0 ,0);
        tTaskSched();
        task2flag = 0;
        tTaskDelay(1);
        task2flag = 1;
        tTaskDelay(1);
    }
}

void task3Entry (void *param)
{
    tEventInit(&eventWaitNormal, tEventTypeUnkonw);
    for (;;) {
        tEventWait(&eventWaitNormal,currentTask,(void *)0, 0 ,0);
        tTaskSched();
        task3flag = 0;
        tTaskDelay(1);
        task3flag = 1;
        tTaskDelay(1);
    }
}

void task4Entry (void *param)
{

    for (;;) {
        tTask *rdytask = tEventWakeUp(&eventWaitNormal,(void *)0,0);
        tTaskSched();
        task4flag = 0;
        tTaskDelay(1);
        task4flag = 1;
        tTaskDelay(1);
    }
}

void tInitApp(void)
{
    tTaskInit (&tTask1, task1Entry, (void *)(0x11111111), 0, &task1Env[1024]);
    tTaskInit (&tTask2, task2Entry, (void *)(0x22222222), 1, &task2Env[1024]);
    tTaskInit (&tTask3, task3Entry, (void *)(0x22222222), 0, &task3Env[1024]);
    tTaskInit (&tTask4, task4Entry, (void *)(0x44444444), 1, &task4Env[1024]);
}
