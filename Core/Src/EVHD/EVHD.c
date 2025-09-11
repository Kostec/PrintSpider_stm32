/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "EVHD/EVHD.h"
#include "LOG/LOG.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define EVHD__MAX_SUBSCRIBERS  8
static EVHD_fpEvHandler EVHD_handlers[EVHD__MAX_SUBSCRIBERS];
#define EVHD_handlersSizeOf (sizeof(EVHD_handlers)/sizeof(EVHD_handlers[0]))

#define EVHD__QUEUE_LEN  16
static osMessageQueueId_t EVHD__messageQueue;
osThreadId_t EVHD__taskHandle;
typedef struct{
    EVHD_tenEvent ev;
} tstEVHD_eventMessage;

osMutexId_t EVHD_sendEventMutex;

void EVHD_Init()
{   
    const osMutexAttr_t EVHD_sendEventMutexAttr = {.name="EVHD_sendEventMutex"};
    EVHD_sendEventMutex = osMutexNew(&EVHD_sendEventMutexAttr);

    const osMessageQueueAttr_t EVHD_Queue_attributes = {
        .name = "EVHD__messageQueue"
    };
    EVHD__messageQueue = osMessageQueueNew(EVHD__QUEUE_LEN, sizeof(tstEVHD_eventMessage), &EVHD_Queue_attributes);
    configASSERT(EVHD__messageQueue);
    osThreadAttr_t EVHD_TaskAttributes = {
        .name = "EVHD_Task",
        .stack_size = 176 * 4,
        .priority = (osPriority_t) osPriorityNormal,
      };
    EVHD__taskHandle = osThreadNew(EVHD_Task, NULL, &EVHD_TaskAttributes);
    configASSERT(EVHD__taskHandle);
}

void EVHD_Task(void *pvParameters)
{   
    tstEVHD_eventMessage ev;
    LOG_Debug("%s", __FUNCTION__);
    for(;;)
    {
        if (osMessageQueueGet(EVHD__messageQueue, &ev, NULL, osWaitForever) == osOK)
        {
            for(uint8_t i = 0; i < EVHD_handlersSizeOf; i++)
            {
                EVHD_fpEvHandler handler = EVHD_handlers[i];
                if (handler)
                {
                    handler(ev.ev);
                }
            }
        }
    }
}

void EVHD_sendEvent(EVHD_tenEvent ev)
{
    osMutexAcquire(EVHD_sendEventMutex, osWaitForever);
    LOG_Debug("%s: %d", __FUNCTION__, ev);
    tstEVHD_eventMessage msg = {.ev=ev};
    osMessageQueuePut(EVHD__messageQueue, &msg, 0, 0);
    osMutexRelease(EVHD_sendEventMutex);
}

void EVHD_subscribeEvent(EVHD_fpEvHandler handler)
{
    for(uint8_t i = 0; i < EVHD_handlersSizeOf; i++)
    {
        if (EVHD_handlers[i] == NULL)
        {
            EVHD_handlers[i] = handler;
            break;
        }
    }
}

void EVHD_unsubscribeEvent(EVHD_fpEvHandler handler)
{
    for(uint8_t i = 0; i < EVHD_handlersSizeOf; i++)
    {
        if (EVHD_handlers[i] == handler)
        {
            EVHD_handlers[i] = NULL;
            break;
        }
        
    }
}
