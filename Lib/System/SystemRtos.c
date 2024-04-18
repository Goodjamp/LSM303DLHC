#include "stdbool.h"

#include "stm32f411xe.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

bool systemInInterrupt(void)
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

BaseType_t systemRtosSemaphoreTake(SemaphoreHandle_t sem, TickType_t timeout)
{
    if (systemInInterrupt()) {
        return xSemaphoreTakeFromISR(sem, NULL);
    } else {
        return xSemaphoreTake(sem, timeout);
    }
}

BaseType_t systemRtosSemaphoreGive(SemaphoreHandle_t sem)
{
    BaseType_t higherPriorityTaskWoken;

    if (systemInInterrupt()) {
        xSemaphoreGiveFromISR(sem, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    } else {
        xSemaphoreGive(sem);
    }

    return higherPriorityTaskWoken;
}

BaseType_t systemRtosQueueSend(QueueHandle_t qUeueH, void *message, TickType_t timeoute)
{
    BaseType_t higherPriorityTaskWoken;
    BaseType_t result;

    if (systemInInterrupt()) {
        result = xQueueSendFromISR(qUeueH, message, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    } else {
        result = xQueueSend(qUeueH, message, timeoute);
    }

    return result;
}

BaseType_t systemRtosQueueReceive(QueueHandle_t qUeueH, void *message, TickType_t timeoute)
{
    BaseType_t higherPriorityTaskWoken;
    BaseType_t result;

    if (systemInInterrupt()) {
        result = xQueueReceiveFromISR(qUeueH, message, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    } else {
        result = xQueueReceive(qUeueH, message, timeoute);
    }

    return result;
}
