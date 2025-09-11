/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "OLED/MENU/MENU_RESOURCES.h"
#include "EVHD/EVHD.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"

/*
 * The module is abstraction for all UI menues handling in OLED
*/

typedef struct
{
    char* text;
    bool isSelected;
    void (*onClick)();
    void (*onDoubleClick)();
    void (*onPress)();
} MENU_tstListItem;

typedef struct 
{   
    struct MENU_tstMenu* parent;
    MENU_tstListItem* selectedItem;
    int selectedItemIdx;
    uint8_t listItemSize;
    void (*Init)();
    void (*Deinit)();
    void (*DrawStatusBar)();
    void (*Draw)();
    void (*ScrollDown)();
    void (*ScrollUp)(); 
    void (*ScrollRight)();
    void (*ScrollLeft)(); 
    void (*preEventHandler)(EVHD_tenEvent ev);
    void (*postEventHandler)(EVHD_tenEvent ev);
} MENU_tstMenu;

void MENU_Init();
void MENU_Denit();
void MENU_Draw();
void MENU_SetActiveMenu(MENU_tstMenu* menu, bool deinitOld);
MENU_tstMenu* MENU_GetActiveMenu();
void MENU_ScrollDown(MENU_tstMenu* menu, MENU_tstListItem* list);
void MENU_ScrollUp(MENU_tstMenu* menu, MENU_tstListItem* list);
void MENU_Exit();
void MENU_SetParent(MENU_tstMenu* parent, MENU_tstMenu* child);
bool MENU_IsUpdateNeeded();
void MENU_Update();
void MENU_SetWait(bool isWaiting);


#endif // MENU_H
