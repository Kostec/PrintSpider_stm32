/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef EVHD_H
#define EVHD_H

#include "stdint.h"

/*
 * The module handles events and calls envent handlers
*/

typedef enum
{
    EVHD_STICK_XStateChanged,
    EVHD_STICK_YStateChanged,
    EVHD_STICK_SWStateChanged,
    tenOfEventSize
} EVHD_tenEvent;

typedef void (*EVHD_fpEvHandler)(EVHD_tenEvent ev);

void EVHD_Init();
void EVHD_Task(void *pvParameters);
void EVHD_sendEvent(EVHD_tenEvent ev);
void EVHD_subscribeEvent(EVHD_fpEvHandler handler);
void EVHD_unsubscribeEvent(EVHD_fpEvHandler handler);

#endif // EVHD_H
