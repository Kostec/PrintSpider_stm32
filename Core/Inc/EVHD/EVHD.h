#ifndef EVHD_H
#define EVHD_H

#include "stdint.h"

typedef enum
{
    EVHD_STICK_StateChanged,
    tenOfEventSize
} tenEVHD_Event;

typedef void (*fpEVHD_Handler)(tenEVHD_Event ev);

void EVHD_Init();
void EVHD_Task(void *pvParameters);
void EVHD_sendEvent(tenEVHD_Event ev);
void EVHD_subscribeEvent(fpEVHD_Handler handler);

#endif // EVHD_H
