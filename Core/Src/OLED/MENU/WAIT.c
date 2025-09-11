/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/WAIT.h"

static uint32_t stateStart = 0;
static uint8_t x0 = 48;
static uint8_t y0 = 24;
static uint8_t x1 = 80;
static uint8_t y1 = 56;
static uint8_t xc = 48+16;
static uint8_t yc = 24+16;
static uint16_t startAngle = 360;
static uint16_t endAngle = 360;

static MENU_tstMenu WAIT = {
    .Init = WAIT_Init,
    .Deinit = WAIT_Deinit,
    .Draw = WAIT_Draw,
};

void WAIT_Init(MENU_tstMenu* parent)
{
    WAIT.parent = (struct MENU_tstMenu*) parent;
    stateStart = HAL_GetTick();
    startAngle = 300;
} 

void WAIT_Deinit()
{
    
}

void WAIT_Draw()
{
    if (HAL_GetTick() - stateStart > 20)
    {
        startAngle -= 15;
        stateStart = HAL_GetTick();
        if (startAngle == 0)
        {
            startAngle = 300;
        }
    }
    endAngle = startAngle + 60;
    ssd1306_FillRectangle(x0-4, y0-4, x1+4, y1+4, Black);
    ssd1306_DrawRectangle(x0-2, y0-2, x1+2, y1+2, White);
    ssd1306_FillCircle(xc, yc, 16, White);
    ssd1306_FillCircle(xc, yc, 12, Black);
    ssd1306_DrawArc(xc, yc, 16, startAngle, endAngle, Black);
    ssd1306_DrawArc(xc, yc, 15, startAngle, endAngle, Black);
    ssd1306_DrawArc(xc, yc, 14, startAngle, endAngle, Black);
    ssd1306_DrawArc(xc, yc, 13, startAngle, endAngle, Black);

    MENU_Update();
}

MENU_tstMenu* WAIT_GetMenu()
{
    return &WAIT;
}
