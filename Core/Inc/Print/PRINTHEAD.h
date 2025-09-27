#ifndef PRINTHEAD_H
#define PRINTHEAD_H

#include "stdint.h"
#include "main.h"

typedef enum {
    PRINTHEAD_typeBlack,
    PRINTHEAD_typeColor
} PRINTHEAD_tenHeadType;

/// @brief Print
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} PRINTHEAD_tstHeadPin;

typedef struct {
    PRINTHEAD_tenHeadType headType;
    PRINTHEAD_tstHeadPin PWRA;   // F3 - Print trigger line.
    PRINTHEAD_tstHeadPin PWRB;   // F5 - Print trigger line.
    PRINTHEAD_tstHeadPin D1;     // Data line for the yellow color.
    PRINTHEAD_tstHeadPin D2;     // Data line for the magenta color.
    PRINTHEAD_tstHeadPin D3;     // Data line for the color cyan. N/A for Black?
    PRINTHEAD_tstHeadPin DCLK;   // Сlock line.
    PRINTHEAD_tstHeadPin CSYNC;  // Data group switching line
    PRINTHEAD_tstHeadPin S1;     // Clock line.
    PRINTHEAD_tstHeadPin S2;     // Clock line.
    PRINTHEAD_tstHeadPin S3;     // Clock line.
    PRINTHEAD_tstHeadPin S4;     // Clock line.
    PRINTHEAD_tstHeadPin S5;     // Clock line.
    PRINTHEAD_tstHeadPin ID;     // ID - unknown how to use it
    PRINTHEAD_tstHeadPin TS;     // Unknown what it is
} PRINTHEAD_tstInstance;

void PRINTHEAD_Init(PRINTHEAD_tstInstance* head, PRINTHEAD_tenHeadType headType);
void PRINTHEAD_Deinit();
void PRINTHEAD_SetColor(PRINTHEAD_tstInstance* head, uint8_t color);
void PRINTHEAD_Process(PRINTHEAD_tstInstance* head);

#endif // PRINTHEAD_H
