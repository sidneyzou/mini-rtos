#ifndef  TINY_OS_H
#define  TINY_OS_H

#include <stdint.h>

#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY      0
#define TINYOS_TASK_STATE_DELAY    (1 << 1)
 
typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack *stack;
    tNode linkNode;
    uint32_t    delayTicks;
    tNode    delayNode;
    uint32_t prio;
    uint32_t state;
    uint32_t slice;
}tTask;


extern tTask *currentTask;
extern tTask *nextTask;

void tTaskRunFrist(void);
void tTaskSwitch(void);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t key);

void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSchedRdy (tTask *task);
void tTaskSchedUnRdy (tTask *task);

void tTimeTaskWait(tTask *task,uint32_t ticks);
void tTimeTaskWakeUp(tTask *task);
void tTaskInit (tTask *task, void (*entry)(void *), void * param, uint32_t prio, tTaskStack * stack);

void tSetSysTickPeriod(uint32_t ms);
#endif
