#include "EVHD/EVHD.h"
#include "LOG/LOG.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define MAX_SUBSCRIBERS  8
static fpEVHD_Handler EVHD_handlers[MAX_SUBSCRIBERS];
#define EVHD_handlersSizeOf (sizeof(EVHD_handlers)/sizeof(EVHD_handlers[0]))

#define EVHD_QUEUE_LEN  16
static osMessageQueueId_t EVHD_Queue;
osThreadId_t EVHD_TaskHandle;
typedef struct{
    tenEVHD_Event ev;
} EVHD_EventMessage;

osMutexId_t EVHD_sendEventMutex;

void EVHD_Init()
{   
    const osMutexAttr_t EVHD_sendEventMutexAttr = {.name="EVHD_sendEventMutex"};
    EVHD_sendEventMutex = osMutexNew(&EVHD_sendEventMutexAttr);

    const osMessageQueueAttr_t EVHD_Queue_attributes = {
        .name = "EVHD_Queue"
    };
    EVHD_Queue = osMessageQueueNew(EVHD_QUEUE_LEN, sizeof(EVHD_EventMessage), &EVHD_Queue_attributes);
    configASSERT(EVHD_Queue);
    osThreadAttr_t EVHD_TaskAttributes = {
        .name = "EVHD_Task",
        .stack_size = 176 * 4,
        .priority = (osPriority_t) osPriorityNormal,
      };
    EVHD_TaskHandle = osThreadNew(EVHD_Task, NULL, &EVHD_TaskAttributes);
    configASSERT(EVHD_TaskHandle);
}

void EVHD_Task(void *pvParameters)
{   
    EVHD_EventMessage ev;
    LOG_Debug("%s", __FUNCTION__);
    for(;;)
    {
        if (osMessageQueueGet(EVHD_Queue, &ev, NULL, osWaitForever) == osOK)
        {
            for(uint8_t i = 0; i < EVHD_handlersSizeOf; i++)
            {
                fpEVHD_Handler handler = EVHD_handlers[i];
                if (handler)
                {
                    handler(ev.ev);
                }
            }
        }
    }
}

void EVHD_sendEvent(tenEVHD_Event ev)
{
    osMutexAcquire(EVHD_sendEventMutex, osWaitForever);
    LOG_Debug("%s: %d", __FUNCTION__, ev);
    EVHD_EventMessage msg = {.ev=ev};
    osMessageQueuePut(EVHD_Queue, &msg, 0, 0);
    osMutexRelease(EVHD_sendEventMutex);
}

void EVHD_subscribeEvent(fpEVHD_Handler handler)
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

void EVHD_unsubscriveEvent(fpEVHD_Handler handler)
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
