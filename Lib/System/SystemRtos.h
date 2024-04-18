#ifndef SYSTEM_RTOS_H
#define SYSTEM_RTOS_H

#include "FreeRTOS.h"
#include "semphr.h"

BaseType_t systemRtosSemaphoreTake(SemaphoreHandle_t sem, TickType_t timeout);
BaseType_t systemRtosSemaphoreGive(SemaphoreHandle_t sem);
BaseType_t systemRtosQueueSend(QueueHandle_t qUeueH, void *message, TickType_t timeoute);
BaseType_t systemRtosQueueReceive(QueueHandle_t qUeueH, void *message, TickType_t timeoute);

#endif
