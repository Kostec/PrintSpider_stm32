#ifndef STICK_H
#define STICK_H
#include <stdint.h>

typedef struct {
    int32_t X;
    int32_t Y;
    uint8_t SW;
} tstStick;

void STICK_Init();
void STICK_Process();
tstStick STICK_GetStick();

#endif // STICK_H