#ifndef  TINY_OS_H
#define  TINY_OS_H

#include <stdint.h>

#include "tLib.h"
#include "tConfig.h"

#include "tEvent.h"

#include "tTask.h"
typedef enum _tError {
    tErrorNoError = 0,
    tErrorTimeout = 1,
}tError;




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
void tTaskSchedRemove(tTask *task);
void tTimeTaskRemove (tTask *task);
void tTaskDelay(uint32_t delay);
void tTaskSched(void);
void tTaskSystemTickHandler(void);


    
void tSetSysTickPeriod(uint32_t ms);
#endif
