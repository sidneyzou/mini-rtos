#ifndef  TINY_OS_H
#define  TINY_OS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack *stack;
    uint32_t    delayTicks;
}tTask;


extern tTask *currentTask;
extern tTask *nextTask;

void tTaskRunFrist(void);
void tTaskSwitch(void);

uint32_t tTaskEnterCritical(void);

void tTaskExitCritical(uint32_t key);
#endif
