#ifndef EVHD_H
#define EVHD_H

#include "stdint.h"

typedef enum
{
    EVHD_STICK_XStateChanged,
    EVHD_STICK_YStateChanged,
    EVHD_STICK_SWStateChanged,
    tenOfEventSize
} tenEVHD_Event;

typedef void (*fpEVHD_Handler)(tenEVHD_Event ev);

void EVHD_Init();
void EVHD_Task(void *pvParameters);
void EVHD_sendEvent(tenEVHD_Event ev);
void EVHD_subscribeEvent(fpEVHD_Handler handler);
void EVHD_unsubscriveEvent(fpEVHD_Handler handler);

#endif // EVHD_H
