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

void tTaskSchedRemove(tTask *task)
{
    tListRemove(&taskTable[task->prio], &(task->linkNode));
    if (tListCount(&taskTable[task->prio]) == 0) {
        tBitmapClear(&taskPrioBitmap, task->prio);
    }
}

void tTimeTaskRemove (tTask *task)
{
    tListRemove(&tTaskDelayList, &(task->delayNode));
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
    
    task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp(tTask *task)
{
    tListRemove(&tTaskDelayList, &(task->delayNode));
    task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void tTaskSystemTickHandler(void)
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

void tTaskDelayInit(void)
{
    tListInit(&tTaskDelayList);
}

tTask tTaskIdle;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

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
  
    tInitApp();
    
    tTaskInit (&tTaskIdle, idleTaskEntry, (void *)(0), TINYOS_PRO_COUNT - 1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
    
    nextTask  = tTaskHighestReady();
    
    tTaskRunFrist();

    return 0;
}
