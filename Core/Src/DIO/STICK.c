#include "DIO/STICK.h"
#include "main.h"
#include "LOG/LOG.h"
#include "DIO/ADC.h"
#include "EVHD/EVHD.h"

static tstStick STICK__instance = {
    .X = {.state = Idle, .direction = 0},
    .Y = {.state = Idle, .direction = 0},
    .SW = 0
};
static bool STICK__stateChanged = false;

bool STICK_getStateChanged()
{
    return STICK__stateChanged;
}

tstStickAxis STICK__ADCtoAxis(uint16_t value)
{
    tstStickAxis stick;
    stick.value = value;
    if (value < 1024)
    {
        stick.direction = -1;
    }
    else 
    {
        stick.direction = 1;
    }

    if ((value >= 2176 && value < 4064)
        || (value >= 32 && value < 1920))
    {
        stick.state = HalfCrossed;
    }
    else if ((value >= 4064 && value <= 4096)
        || (value >= 0 && value < 32))
    {
        stick.state = Full;
    }
    else
    {
        stick.state = Idle;
        stick.direction = 0;
    }
    return stick;
}

bool STICK__readADC(tstStickAxis* axis, uint16_t value)
{
    tstStickAxis readAxis = STICK__ADCtoAxis(value);

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
    STICK__instance.SW = !HAL_GPIO_ReadPin(STICK_SW_GPIO_Port, STICK_SW_Pin);


    // LOG_Debug("%s: SW=%u", __FUNCTION__, STICK__instance.SW);
}

tstStick STICK_GetStick()
{
    return STICK__instance;
}