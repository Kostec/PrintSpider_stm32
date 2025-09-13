#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    MOTORCTRL_LEFT = 0,
    MOTORCTRL_RIGHT
} MOTORCTRL_tenDirection;

typedef enum
{
    MOTORCTRL_CARRET = 0,
    MOTORCTRL_PAPER
} MOTORCTRL_tenMotor;

void MOTORCTRL_Init();
void MOTORCTRL_Deinit();
void MOTORCTRL_Task(void *pvParameters);

void MOTORCTRL_enable(bool isEnable);
bool MOTORCTRL_isEnable(bool isEnable);
void MOTORCTRL_setDirection(MOTORCTRL_tenMotor motor, MOTORCTRL_tenDirection direction);
void MOTORCTRL_runMotor(MOTORCTRL_tenMotor motor);
void MOTORCTRL_stopMotor(MOTORCTRL_tenMotor motor);

#endif // MOTOR_H
