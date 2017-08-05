#include "tinyOs.h"

void tTaskDelay(uint32_t delay)
{
    uint32_t key = tTaskEnterCritical();
    
    tTimeTaskWait(currentTask, delay);
    
    tTaskSchedUnRdy(currentTask);
    tTaskExitCritical(key);
    tTaskSched();
    
}

