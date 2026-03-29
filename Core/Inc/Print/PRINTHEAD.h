#ifndef PRINTHEAD_H
#define PRINTHEAD_H

#include "stdint.h"
#include "stdbool.h"
#include "main.h"

/*
    PIXEL_CMYK pix;
    pix.value = 0xAABBCCDD;
    uint8_t c = pix.C; // 0xDD
    uint8_t m = pix.M; // 0xCC
    uint8_t y = pix.Y; // 0xBB
    uint8_t k = pix.K; // 0xAA
*/
typedef union {
        struct {
            uint8_t C;
            uint8_t M;
            uint8_t Y;
            uint8_t K;
        } CMYK;
        uint32_t value;;
} PRINTHEAD_tunColor;

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
void PRINTHEAD_SetLineColor(PRINTHEAD_tstInstance* head, PRINTHEAD_tunColor color);
void PRINTHEAD_Process(PRINTHEAD_tstInstance* head);

void PRINTHEAD_Start();
void PRINTHEAD_Stop();
bool PRINTHEAD_IsStarted();

#endif // PRINTHEAD_H
