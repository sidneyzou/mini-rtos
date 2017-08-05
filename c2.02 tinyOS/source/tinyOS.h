#ifndef  TINY_OS_H
#define  TINY_OS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack *stack;
}tTask;


extern tTask *currentTask;
extern tTask *nextTask;

void tTaskRunFrist(void);
void tTaskSwitch(void);
#endif
