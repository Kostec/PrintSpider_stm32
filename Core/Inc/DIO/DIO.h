/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef DIO_H
#define DIO_H

#include "stdint.h"

/*
 * The module handles digital I/O ports
 */

#define PCF8574_I2C_ADDR (0x20 << 1)
#define PCF8574_I2C_PORT hi2c3

typedef enum
{
    P0,
    P1,
    P2,
    P3,
    P4,
    P5,
    P6,
    P7,
} DIO_tenPin;

typedef union {
    uint8_t byte;
    struct {
        uint8_t P0 : 1;
        uint8_t P1 : 1;
        uint8_t P2 : 1;
        uint8_t P3 : 1;
        uint8_t P4 : 1;
        uint8_t P5 : 1;
        uint8_t P6 : 1;
        uint8_t P7 : 1;
    } bits;
} DIO_tstPCF8574_Data;

void DIO_Init();
void DIO_Task(void *pvParameters);
uint8_t DIO_ReadPin(DIO_tenPin pin);
void DIO_WritePin(DIO_tenPin pin, uint8_t value);

DIO_tstPCF8574_Data DIO_ReadAll();

#endif //DIO_H