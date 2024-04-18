#include "FreeRTOS.h"
#include "task.h"

#include "SystemClock.h"
#include "DebugServices.h"

void main(void)
{
    /* Configure the system clock */
    systemClockInit();
    debugServicesInit(NULL);

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */

   for (;;){}
}

