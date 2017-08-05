#include "tEvent.h"

void tEventInit (tEvent *event, tEventType type)
{
    event->type = tEventTypeUnkonw;
    tListInit(&event->waitList);
}
 
 
