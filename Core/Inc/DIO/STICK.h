#ifndef STICK_H
#define STICK_H
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Idle,
    Move,
    Hold,
} tenStickAxisState;

typedef struct {
    uint32_t value;
    tenStickAxisState state;
    int8_t direction;
} tstStickAxis;

typedef struct {
    tstStickAxis X;
    tstStickAxis Y;
    uint8_t SW;
} tstStick;

void STICK_Init();
void STICK_Process();
tstStick STICK_GetStick();
void STICK_HandleADC(uint16_t* buffer);
bool STICK_getStateChanged();

#endif // STICK_H