/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/MENU.h"
#include "OLED/MENU/WAIT.h"
#include "DIO/STICK.h"
#include <stdio.h>


static MENU_tstMenu* MENU__activeMenu;
extern RTC_HandleTypeDef hrtc;
static bool MENU__isUpdateNeeded;
static bool MENU__isWaiting;
void MENU__EventHandler(EVHD_tenEvent ev);

void MENU__StatusTime()
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    char tmp[16] = {0};
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    sprintf(tmp, "%02u:%02u:%02u", time.Hours, time.Minutes, time.Seconds);

    ssd1306_SetCursor(0,2);
    ssd1306_WriteString(tmp, Font_7x10, White);
}

void MENU__StatusSdCard(bool show)
{
    if(show)
    {
        ssd1306_DrawBitmap(113,0, micro_sd_15x15, 15,15, White);
    }
    else
    {
        ssd1306_FillRectangle(113,0, 128, 15, Black);
    }
}

void MENU__StatusUSB(bool show)
{
    if(show)
    {
        ssd1306_DrawBitmap(97,0, image_data_USB_15x15, 15,15, White);
    }
    else
    {
        ssd1306_FillRectangle(97,0, 128, 15, Black);
    }
}

void MENU__DrawStatusBar()
{
    if (MENU__activeMenu->DrawStatusBar)
    {
        MENU__activeMenu->DrawStatusBar();
    }
    else
    {
        MENU__StatusTime();
        MENU__StatusSdCard(true);
        MENU__StatusUSB(true);
    }
}

void MENU_Init()
{
    // MENU__activeMenu = NULL;
    EVHD_subscribeEvent(MENU__EventHandler);
    MENU__isUpdateNeeded = true;
}

void MENU_Denit()
{
    // MENU__activeMenu = NULL;
    EVHD_unsubscribeEvent(MENU__EventHandler);
}

void MENU_Draw()
{
    if (!MENU__isUpdateNeeded)
    {
        return;
    }

    MENU__DrawStatusBar();

    if (MENU__activeMenu)
    {
        MENU__activeMenu->Draw();
    }
    if (MENU__isWaiting)
    {
        WAIT_Draw();
        return;
    }
    MENU__isUpdateNeeded = false;
}

void MENU_SetActiveMenu(MENU_tstMenu* menu, bool deinitOld)
{
    if (deinitOld)
    {
        MENU__activeMenu->Deinit();
    }
    MENU__activeMenu = menu;
    MENU_Update();
}

MENU_tstMenu* MENU_GetActiveMenu()
{
    return MENU__activeMenu;
}

void MENU_StickAction()
{
    STICK_tstStick stick = STICK_GetStick();
    if (stick.Y.direction == -1 && stick.Y.state == Move
        && MENU__activeMenu->ScrollDown)
    {
        MENU__activeMenu->ScrollDown();
    }
    else if (stick.Y.direction == 1 && stick.Y.state == Move
            && MENU__activeMenu->ScrollUp)
    {
        MENU__activeMenu->ScrollUp();
    }
    if (stick.X.direction == 1 && stick.X.state == Move
        && MENU__activeMenu->ScrollLeft)
    {
        MENU__activeMenu->ScrollLeft();
    }
    else if (stick.X.direction == -1 && stick.X.state == Move
            && MENU__activeMenu->ScrollRight)
    {
        MENU__activeMenu->ScrollRight();
    }
}

void MENU_ClickAction()
{
    // STICK_tstStick stick = STICK_GetStick();
}

void MENU__EventHandler(EVHD_tenEvent ev)
{
    if (!MENU__activeMenu || MENU__isWaiting)
    {
        return;
    }

    if (MENU__activeMenu->preEventHandler)
    {
        MENU__activeMenu->preEventHandler(ev);
    }

    if (ev == EVHD_STICK_YStateChanged)
    {
        MENU_StickAction();
    }
    else if (ev == EVHD_STICK_XStateChanged)
    {
        MENU_StickAction();
    }
    else if (ev == EVHD_STICK_SWStateChanged)
    {
        STICK_tstStick stick = STICK_GetStick();
        if (stick.SW == 1 && MENU__activeMenu->selectedItem && MENU__activeMenu->selectedItem->onClick)
        {

            MENU__activeMenu->selectedItem->onClick();
        }
    }

    if (MENU__activeMenu->postEventHandler)
    {
        MENU__activeMenu->postEventHandler(ev);
    }

}

void MENU_ScrollDown(MENU_tstMenu* menu, MENU_tstListItem* list)
{
    if (menu->selectedItemIdx + 1 < menu->listItemSize)
    {
        list[menu->selectedItemIdx].isSelected = false;
        menu->selectedItemIdx++;
        list[menu->selectedItemIdx].isSelected = true;
        menu->selectedItem = &list[menu->selectedItemIdx];
    }
    MENU_Update();
}

void MENU_ScrollUp(MENU_tstMenu* menu, MENU_tstListItem* list)
{
    if (menu->selectedItemIdx - 1 >= 0)
    {
        list[menu->selectedItemIdx].isSelected = false;
        menu->selectedItemIdx--;
        list[menu->selectedItemIdx].isSelected = true;
        menu->selectedItem = &list[menu->selectedItemIdx];
    }
    MENU_Update();
}

void MENU_Exit()
{
    MENU_SetActiveMenu((MENU_tstMenu*)MENU__activeMenu->parent, true);
}

void MENU_SetParent(MENU_tstMenu* parent, MENU_tstMenu* child)
{
    child->parent = (struct MENU_tstMenu*) parent;
}

bool MENU_IsUpdateNeeded()
{
    return MENU__isUpdateNeeded;
}

void MENU_Update()
{
    MENU__isUpdateNeeded = true;
}

void MENU_SetWait(bool isWaiting)
{
    if (!MENU__isWaiting && isWaiting)
    {
        WAIT_Init(NULL);
    }
    MENU__isWaiting = isWaiting;
}
