/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef STICK_H
#define STICK_H
#include <stdint.h>
#include <stdbool.h>

/*
 * The module handles Stick module state
*/

typedef enum {
    Idle,
    Move,
    Hold,
} STICK_tenAxisState;

typedef struct {
    uint32_t value;
    STICK_tenAxisState state;
    int8_t direction;
} STICK_tstAxis;

typedef struct {
    STICK_tstAxis X;
    STICK_tstAxis Y;
    uint8_t SW;
} STICK_tstStick;

void STICK_Init();
void STICK_Process();
STICK_tstStick STICK_GetStick();
void STICK_HandleADC(uint16_t* buffer);
bool STICK_getStateChanged();

#endif // STICK_H