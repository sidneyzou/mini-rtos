#include "tinyOS.h"
#include "ARMCM3.h"


void tTaskSched(void);


tTask *currentTask;
tTask *nextTask;
tTask *idleTask;

tBitmap taskPrioBitmap;
tList taskTable[TINYOS_PRO_COUNT];

uint8_t schedLockCount;

tList tTaskDelayList;
void tTaskInit (tTask *task, void (*entry)(void *), void * param, uint32_t prio, tTaskStack * stack)
{
    *(--stack) = (unsigned long)(1 << 24);
    *(--stack) = (unsigned long)entry;
    *(--stack) = (unsigned long)0x14;
    *(--stack) = (unsigned long)0x12;
    *(--stack) = (unsigned long)0x3;
    *(--stack) = (unsigned long)0x2;
    *(--stack) = (unsigned long)0x1;
    *(--stack) = (unsigned long)param;

    *(--stack) = (unsigned long)0x11;
    *(--stack) = (unsigned long)0x10;
    *(--stack) = (unsigned long)0x9;
    *(--stack) = (unsigned long)0x8;
    *(--stack) = (unsigned long)0x7;
    *(--stack) = (unsigned long)0x6;
    *(--stack) = (unsigned long)0x5;
    *(--stack) = (unsigned long)0x4;

    task->slice = TINYOS_SLICK_MAX;
    task->stack = stack; 
    task->delayTicks = 0;
    task->prio = prio;
    task->state = TINYOS_TASK_STATE_RDY;
    
    tNodeInit(&task->delayNode);
    tNodeInit(&task->linkNode);   
    tListAddFirst(&taskTable[prio], &(task->linkNode));
    
    tBitmapSet(&taskPrioBitmap, prio);
}

tTask * tTaskHighestReady (void)
{
    uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
    tNode *node = tListFirst(&taskTable[highestPrio]);
    return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit(void)
{
    int i = 0;
    schedLockCount = 0;
    tBitmapInit(&taskPrioBitmap);
    for (i = 0; i < TINYOS_PRO_COUNT; i++) {
        tListInit(&taskTable[i]);
    }
}

void tTaskSchedDisable(void)
{
    uint32_t key = tTaskEnterCritical();
    
    if (schedLockCount < 255) {
        schedLockCount++;
    }
    
    tTaskExitCritical(key);
}


void tTaskSchedEnable(void)
{
        uint32_t key = tTaskEnterCritical();
    
    if (schedLockCount > 0) {
        if (--schedLockCount == 0) {
            tTaskSched();
        }
    }
    
    tTaskExitCritical(key);
}

void tTaskSchedRdy (tTask *task)
{
    tListAddFirst(&taskTable[task->prio], &(task->linkNode));
    tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy (tTask *task)
{
    tListRemove(&taskTable[task->prio], &(task->linkNode));
    if (tListCount(&taskTable[task->prio]) == 0) {
        tBitmapClear(&taskPrioBitmap, task->prio);
    }
    
}
void tTaskSched(void)
{
    tTask *tempTask;
    uint32_t key = tTaskEnterCritical();
    
    if (schedLockCount > 0) {
        tTaskExitCritical(key);
        return;
    }
    tempTask = tTaskHighestReady();
    if (tempTask != currentTask) {
        nextTask = tempTask;
        tTaskSwitch();
    }
    
    tTaskExitCritical(key);  
}

void tTimeTaskWait(tTask *task,uint32_t ticks)
{
    task->delayTicks = ticks;
    tListAddLast(&tTaskDelayList, &(task->delayNode));
    
    task->state |= TINYOS_TASK_STATE_DELAY;
}

void tTimeTaskWakeUp(tTask *task)
{
    tListRemove(&tTaskDelayList, &(task->delayNode));
    task->state &= ~TINYOS_TASK_STATE_DELAY;
}

void tTaskSystemTickHandler()
{
    tNode *node;
    uint32_t key = tTaskEnterCritical();
    
    for(node = tTaskDelayList.headNode.nextNode; node != &(tTaskDelayList.headNode); node = node->nextNode) {
        tTask *task = tNodeParent(node, tTask, delayNode);
        if (--task->delayTicks == 0) {
            tTimeTaskWakeUp(task);
            tTaskSchedRdy(task);
        }
    }
    
    if (--currentTask->slice == 0) {
        if (tListCount(&taskTable[currentTask->prio]) > 0) {
            tListRemoveFirst(&taskTable[currentTask->prio]);
            tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
            currentTask->slice = TINYOS_SLICK_MAX;
        }
    }
    
    tTaskExitCritical(key);
    tTaskSched();
    
}

void tTaskDelay(uint32_t delay)
{
    uint32_t key = tTaskEnterCritical();
    
    tTimeTaskWait(currentTask, delay);
    
    tTaskSchedUnRdy(currentTask);
    tTaskExitCritical(key);
    tTaskSched();
    
}


void tTaskDelayInit(void)
{
    tListInit(&tTaskDelayList);
}

void tSetSysTickPeriod(uint32_t ms)
{
    SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
    tTaskSystemTickHandler();
}

int task1flag;
void task1Entry (void *param)
{ 
    tSetSysTickPeriod(10);
    for (;;) {
        
        task1flag = 0;
        tTaskDelay(1);
        task1flag = 1;
        tTaskDelay(1);
    }
}
void delay(void)
{
    int i = 0;
    for (i = 0;i < 0xff;i++) ;
}

int task2flag;
void task2Entry (void *param)
{
    for (;;) {
        task2flag = 0;
        delay();
        task2flag = 1;
        delay();
    }
}

int task3flag;
void task3Entry (void *param)
{
    for (;;) {
        task3flag = 0;
        delay();
        task3flag = 1;
        delay();
    }
}

tTask tTask1;
tTask tTask2;
tTask tTask3;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];

tTask tTaskIdle;
tTaskStack idleTaskEnv[1024];

void idleTaskEntry(void *param)
{
    for (;;)
    {
    }
}

int main ()
{
    tTaskSchedInit();
    
    tTaskDelayInit();
    tTaskInit (&tTask1, task1Entry, (void *)(0x11111111), 0, &task1Env[1024]);
    tTaskInit (&tTask2, task2Entry, (void *)(0x22222222), 1, &task2Env[1024]);
    tTaskInit (&tTask3, task3Entry, (void *)(0x22222222), 1, &task3Env[1024]);
    tTaskInit (&tTaskIdle, idleTaskEntry, (void *)(0), TINYOS_PRO_COUNT - 1, &idleTaskEnv[1024]);
    
    nextTask  = tTaskHighestReady();
    
    tTaskRunFrist();

    return 0;
}
