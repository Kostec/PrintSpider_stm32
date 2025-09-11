/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/MAIN_MENU.h"
#include "OLED/MENU/MENU_RESOURCES.h"

#include "DIO/STICK.h"
#include "EVHD/EVHD.h"
#include "stdio.h"

#include "OLED/MENU/SERVICE_MENU.h"
#include "OLED/MENU/SD_MENU.h"

void MAIN_MENU_openServiceMenu();
void MAIN_MENU_openSdMenu();

void MAIN_MENU_STICK_STATE();
void MAIN_MENU_StickAction();
void MAIN_MENU_EventHandler(EVHD_tenEvent ev);

static MENU_tstListItem MENU_mainListItems []= 
{
    {
        .text = "Print",
    },
    {
        .text = "SD",
        .onClick = MAIN_MENU_openSdMenu
    },
    {
        .text = "Service",
        .onClick = MAIN_MENU_openServiceMenu
    },
    {
        .text = "About"
    },
    {
        .text = "Restart"
    }
};

// static bool showSDCard = true;
static MENU_tstMenu MAIN_MENU = {
    .Init = MAIN_MENU_Init,
    .Deinit = MAIN_MENU_Deinit,
    .Draw = MAIN_MENU_Draw,
    .ScrollUp = MAIN_MENU_ScrollUp,
    .ScrollDown = MAIN_MENU_ScrollDown,
    .listItemSize = sizeof(MENU_mainListItems)/sizeof(MENU_tstListItem)
};

void MAIN_MENU_openServiceMenu()
{
    SERVICE_MENU_Init(&MAIN_MENU);
    MENU_SetActiveMenu(SERVICE_MENU_GetMenu(), false);
}

void MAIN_MENU_openSdMenu()
{
    SD_MENU_Init(&MAIN_MENU);
    MENU_SetActiveMenu(SD_MENU_GetMenu(), false);
}

void MAIN_MENU_drawListItem(MENU_tstListItem listItem, uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(listItem.text, Font_7x10, listItem.isSelected ? Black : White);
}

void MAIN_MENU_List()
{
    uint8_t startLine = 16;
    uint8_t startItemIdx = MAIN_MENU.selectedItemIdx > 1 ? MAIN_MENU.selectedItemIdx - 1 : 0;
    for(uint8_t i = 0; 
        (i < sizeof(MENU_mainListItems)/sizeof(MENU_mainListItems[0])) || (startLine + i*10 > 128);
        i++)
    {
        MAIN_MENU_drawListItem(MENU_mainListItems[startItemIdx + i], 0, 16 + i*10);
    }
    
}

void MAIN_MENU_Init()
{
    MAIN_MENU.selectedItemIdx = 0;
    MAIN_MENU.selectedItem = &MENU_mainListItems[0];
    MAIN_MENU.selectedItem->isSelected = true;
}

void MAIN_MENU_Deinit()
{
    
}

void MAIN_MENU_Draw()
{
    MAIN_MENU_List();
}

void MAIN_MENU_ScrollUp()
{
    MENU_ScrollUp(&MAIN_MENU, MENU_mainListItems);
}

void MAIN_MENU_ScrollDown()
{
    MENU_ScrollDown(&MAIN_MENU, MENU_mainListItems);
}

MENU_tstMenu* MAIN_MENU_GetMenu()
{
    return &MAIN_MENU;
}

