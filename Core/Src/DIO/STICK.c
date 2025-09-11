/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "DIO/STICK.h"
#include "main.h"
#include "LOG/LOG.h"
#include "DIO/ADC.h"
#include "EVHD/EVHD.h"
#include <stdbool.h>

#define STICK__debounceValue (5)

static STICK_tstStick STICK__instance = {
    .X = {.state = Idle, .direction = 0},
    .Y = {.state = Idle, .direction = 0},
    .SW = 0
};
static bool STICK__stateChanged = false;

static uint8_t STICK__debounceCounter = STICK__debounceValue;

void STICK__debounceSW()
{
    static uint8_t oldSW = 0;
    static uint8_t newSW = 0;
    static bool debouncing = false; 

    oldSW = STICK__instance.SW;
    newSW = !HAL_GPIO_ReadPin(STICK_SW_GPIO_Port, STICK_SW_Pin);

    if (newSW != oldSW)
    {
        if (STICK__debounceCounter == 0 && !debouncing)
        {
            STICK__debounceCounter = STICK__debounceValue;
            debouncing = true;
            return;
        }

        STICK__debounceCounter--;
        if (STICK__debounceCounter == 0 && debouncing)
        {
            STICK__instance.SW = newSW;
            EVHD_sendEvent(EVHD_STICK_SWStateChanged);
            debouncing = false;
            return;
        }

    }

    STICK__instance.SW = oldSW;
}

bool STICK_getStateChanged()
{
    return STICK__stateChanged;
}

STICK_tstAxis STICK__ADCtoAxis(uint16_t value)
{
    STICK_tstAxis stick;
    stick.value = value;
    if (value < 1536)
    {
        stick.direction = -1;
        stick.state = Move;
    }
    else if (value >= 2560)
    {
        stick.direction = 1;
        stick.state = Move;
    }
    else
    {
        stick.state = Idle;
        stick.direction = 0;
    }

    return stick;
}

bool STICK__readADC(STICK_tstAxis* axis, uint16_t value)
{
    STICK_tstAxis readAxis = STICK__ADCtoAxis(value);

    axis->value = readAxis.value;
    if (axis->state != readAxis.state
        || axis->direction != readAxis.direction
    )
    {
        axis->direction = readAxis.direction;
        axis->state = readAxis.state;
        return true;
    }
    else
    {
        return false;
    }
}

void STICK_Init()
{
}

void STICK_Process()
{
    if (STICK__readADC(&STICK__instance.X, ADC_GetValue(0)))
    {
        LOG_Debug("%s: X={dir=%d state=%lu value=%lu}", __FUNCTION__, STICK__instance.X.direction, STICK__instance.X.state, STICK__instance.X.value);
        EVHD_sendEvent(EVHD_STICK_XStateChanged);
    }
    if(STICK__readADC(&STICK__instance.Y, ADC_GetValue(1)))
    {
        LOG_Debug("%s: Y={dir=%d state=%lu value=%lu}", __FUNCTION__, STICK__instance.Y.direction, STICK__instance.Y.state, STICK__instance.Y.value);
        EVHD_sendEvent(EVHD_STICK_YStateChanged);
    }

    
    STICK__debounceSW();


    // LOG_Debug("%s: SW=%u", __FUNCTION__, STICK__instance.SW);
}

STICK_tstStick STICK_GetStick()
{
    return STICK__instance;
}