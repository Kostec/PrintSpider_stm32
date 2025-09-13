#include "Print/PRINTCTRL.h"
#include "main.h"
#include "MOTOR/MOTOR.h"
#include "LOG/LOG.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>

static osThreadId_t PRINTCTRL__Thread;

void PRINTCTRL_Init()
{
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

    for(;;)
    {
        osDelay(10);
    }
}
