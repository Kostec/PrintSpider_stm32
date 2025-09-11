/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/SERVICE_MENU/S_IO_MENU.h"
#include "DIO/DIO.h"
#include "DIO/ADC.h"
#include "DIO/STICK.h"
#include <stdio.h>
#include <string.h>

static MENU_tstListItem S_IO_MENU_listItems []= 
{
    {
        .text = "../",
        .onClick = MENU_Exit
    },
};

static MENU_tstMenu S_IO_MENU =
{
    .Init = S_IO_MENU_Init,
    .Deinit = S_IO_MENU_Deinit,
    .Draw = S_IO_MENU_Draw,
    .ScrollUp = S_IO_MENU_ScrollUp,
    .ScrollDown = S_IO_MENU_ScrollDown,
    .listItemSize = sizeof(S_IO_MENU_listItems)/sizeof(MENU_tstListItem)
};

static char lines[16][32] = {0};
static uint8_t startLine = 0;
static uint8_t linesCount = 0;

void S_IO_MENU_Init(MENU_tstMenu* parent)
{
    S_IO_MENU.parent = (struct MENU_tstMenu*) parent;
    S_IO_MENU.selectedItemIdx = 0;
    S_IO_MENU.selectedItem = &S_IO_MENU_listItems[0];
    S_IO_MENU.selectedItem->isSelected = true;

    startLine = 0;
    linesCount = 0;
}

void S_IO_MENU_Deinit()
{
    S_IO_MENU.parent = NULL;
}

void S_IO_MENU__PrepareLines()
{
    memset(lines, 0, 16*32);
    char* line = lines[0];
    strcat(line, "DIO:");

    line = lines[1];
    DIO_tstPCF8574_Data dioData = DIO_ReadAll();
    sprintf(line, "%d %d %d %d %d %d %d %d",
        dioData.bits.P0,
        dioData.bits.P1,
        dioData.bits.P2,
        dioData.bits.P3,
        dioData.bits.P4,
        dioData.bits.P5,
        dioData.bits.P6,
        dioData.bits.P7);

    line = lines[2];
    strcat(line, "ADC:");
    line = lines[3];
    strcat(line, "N/A");

    line = lines[4];
    strcat(line, "Stick:");
    line = lines[5];
    STICK_tstStick stick = STICK_GetStick();
    sprintf(line, "X: %d %d %lu", stick.X.direction, stick.X.state, stick.X.value);
    line = lines[6];
    sprintf(line, "Y: %d %d %lu", stick.Y.direction, stick.Y.state, stick.Y.value);
    
    linesCount = 7;
}

void S_IO_MENU_Draw()
{
    S_IO_MENU__PrepareLines();
    const uint8_t lineOffset = 10;

    for(uint8_t i = 0; i < linesCount && (16 + i*lineOffset*2 < 128); i++)
    {
        ssd1306_SetCursor(0, 16 + i*lineOffset);
        ssd1306_WriteString(lines[startLine+i], Font_7x10, White);
    }
}

void S_IO_MENU_ScrollUp()
{
    if (startLine - 1 >= 0)
    {
        startLine--;
        MENU_Update();
    }
}

void S_IO_MENU_ScrollDown()
{
    if (startLine + 1 <= linesCount)
    {
        startLine++;
        MENU_Update();
    }
}

MENU_tstMenu* S_IO_MENU_GetMenu()
{
    return &S_IO_MENU;
}
