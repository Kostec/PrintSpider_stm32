#include "EVHD/EVHD.h"
#include "LOG/LOG.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define MAX_SUBSCRIBERS  8
static fpEVHD_Handler EVHD_handlers[MAX_SUBSCRIBERS];
static uint8_t EVHD_handlersCount = 0;

#define BUS_QUEUE_LEN  16
static osMessageQueueId_t EVHD_Queue;
const osMessageQueueAttr_t EVHD_Queue_attributes = {
    .name = "EVHD_Queue"
  };

osThreadId_t EVHD_TaskHandle;
const osThreadAttr_t EVHD_TaskAttributes = {
  .name = "EVHD_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

typedef struct{
    tenEVHD_Event ev;
} EVHD_EventMessage;

void EVHD_Init()
{
    EVHD_Queue = osMessageQueueNew(BUS_QUEUE_LEN, sizeof(EVHD_EventMessage), &EVHD_Queue_attributes);
    configASSERT(EVHD_Queue);
    EVHD_TaskHandle = osThreadNew(EVHD_Task, NULL, &EVHD_TaskAttributes);
    configASSERT(EVHD_TaskHandle);
}

void EVHD_Task(void *pvParameters)
{   
    // EVHD_EventMessage ev;
    LOG_Debug("%s", __FUNCTION__);
    for(;;)
    {
        // if (osMessageQueueGet(EVHD_Queue, &ev, NULL, osWaitForever) == osOK)
        // {
        //     for(uint8_t i = 0; i < EVHD_handlersCount; i++)
        //     {
        //         fpEVHD_Handler handler = EVHD_handlers[i];
        //         if (handler)
        //         {
        //             handler(ev.ev);
        //         }
        //     }
        // }
        osDelay(100);
    }
}

void EVHD_sendEvent(tenEVHD_Event ev)
{
    // LOG_Debug("%s: %d", __FUNCTION__, ev);
    // EVHD_EventMessage msg = {.ev=ev};
    // osMessageQueuePut(EVHD_Queue, &msg, 0, 0);
}

void EVHD_subscribeEvent(fpEVHD_Handler handler)
{
    EVHD_handlers[EVHD_handlersCount] = handler;
    EVHD_handlersCount++;
}
