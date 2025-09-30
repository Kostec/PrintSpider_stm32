#include "Print/PRINTCTRL.h"
#include "Print/PRINTHEAD.h"
#include "main.h"
#include "MOTOR/MOTOR.h"
#include "LOG/LOG.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>

static osThreadId_t PRINTCTRL__Thread;
static PRINTHEAD_tstInstance PRINTCTRL__blackHead;

void PRINTCTRL_Init()
{
    PRINTHEAD_Init(&PRINTCTRL__blackHead, PRINTHEAD_typeBlack);

    const osThreadAttr_t PRINTCTRL_attributes = {
        .name = "PRINTCTRL_Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
      PRINTCTRL__Thread = osThreadNew(PRINTCTRL_Task, NULL, &PRINTCTRL_attributes);
    configASSERT(PRINTCTRL__Thread);
}

void PRINTCTRL_Deinit()
{

}

void PRINTCTRL_Task(void *pvParameters)
{
    MOTORCTRL_enable(false);
    MOTORCTRL_runMotor(MOTORCTRL_CARRET);
    MOTORCTRL_runMotor(MOTORCTRL_PAPER);

    PRINTHEAD_tunColor color = {0};
    PRINTHEAD_SetLineColor(&PRINTCTRL__blackHead, color);
    PRINTHEAD_Start();
    for(;;)
    {
        if (!PRINTHEAD_IsStarted())
        {
            PRINTHEAD_SetLineColor(&PRINTCTRL__blackHead, color);
            PRINTHEAD_Start();
        }
        osDelay(1000);
        PRINTHEAD_Stop();
    }
}
