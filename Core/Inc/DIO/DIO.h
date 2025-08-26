#ifndef DIO_H
#define DIO_H

#include "stdint.h"

#define PCF8574_I2C_ADDR (0x20 << 1)
#define PCF8574_I2C_PORT hi2c3

void DIO_Init();
void DIO_Task(void *pvParameters);

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
} PCF8574_Data;

#endif //DIO_H