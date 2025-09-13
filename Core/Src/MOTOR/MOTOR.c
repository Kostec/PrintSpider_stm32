#include "main.h"
#include "MOTOR/MOTOR.h"
#include "LOG/LOG.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
    GPIO_TypeDef* dirPort;
    uint16_t dirPin;
    MOTORCTRL_tenDirection direction;
    bool run;
} MOTORCTRL_tstMotor;

#define MOTOR_MotorCount (sizeof(MOTORCTRL__motors)/sizeof(MOTORCTRL_tstMotor))

static osThreadId_t MOTORCTRL__Thread;

static bool MOTORCTRL__isEnabled = false;
static MOTORCTRL_tstMotor MOTORCTRL__motors[] = {
    /* CARRET */    {.direction = MOTORCTRL_LEFT, .port = MOTOR_X_STEP_GPIO_Port, .pin = MOTOR_X_STEP_Pin, .dirPort = MOTOR_X_DIR_GPIO_Port, .dirPin = MOTOR_X_DIR_Pin, .run = false},
    /* PAPER */     {.direction = MOTORCTRL_LEFT, .port = MOTOR_Y_STEP_GPIO_Port, .pin = MOTOR_Y_STEP_Pin, .dirPort = MOTOR_Y_DIR_GPIO_Port, .dirPin = MOTOR_Y_DIR_Pin, .run = false}
};

void MOTORCTRL_Init()
{
    const osThreadAttr_t MOTORCTRL_attributes = {
        .name = "MOTORCTRL_Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
      MOTORCTRL__Thread = osThreadNew(MOTORCTRL_Task, NULL, &MOTORCTRL_attributes);
    configASSERT(MOTORCTRL__Thread);
    MOTORCTRL__isEnabled = false;
}

void MOTORCTRL_Deinit()
{

}

void MOTORCTRL_Task(void *pvParameters)
{
    // LOG_Debug("%s", __FUNCTION__);
    MOTORCTRL_tenMotor activeMotor = 0;
    for(;;)
    {   
        HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, MOTORCTRL__isEnabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
        if (!MOTORCTRL__isEnabled)
        {
            osDelay(1);
            continue;
        }

        activeMotor++;
        if (activeMotor == MOTOR_MotorCount)
        {
            activeMotor = 0;
        }

        MOTORCTRL_tstMotor mot = MOTORCTRL__motors[activeMotor];
        if (!mot.run)
        {
            continue;;
        }

        HAL_GPIO_WritePin(mot.dirPort, mot.dirPin, mot.direction == MOTORCTRL_LEFT ? GPIO_PIN_SET : GPIO_PIN_RESET);

        HAL_GPIO_TogglePin(mot.port, mot.pin);

        osDelay(1);
    }
}

void MOTORCTRL_enable(bool isEnable)
{
    MOTORCTRL__isEnabled = isEnable;

    HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, isEnable ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

bool MOTORCTRL_isEnable(bool isEnable)
{
    return MOTORCTRL__isEnabled;
}

void MOTORCTRL_setDirection(MOTORCTRL_tenMotor motor, MOTORCTRL_tenDirection direction)
{
    MOTORCTRL__motors[motor].direction = direction;
}

void MOTORCTRL_runMotor(MOTORCTRL_tenMotor motor)
{
    MOTORCTRL__motors[motor].run = true;
}

void MOTORCTRL_stopMotor(MOTORCTRL_tenMotor motor)
{
    MOTORCTRL__motors[motor].run = false;
}



