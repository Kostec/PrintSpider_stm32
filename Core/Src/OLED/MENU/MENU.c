#include "MENU/MENU.h"
#include "DIO/STICK.h"
#include <stdio.h>


static tstMENU_menu* MENU_activeMenu;
extern RTC_HandleTypeDef hrtc;
static bool IsUpdateNeeded;
void SERVICE_MENU_EventHandler(tenEVHD_Event ev);

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
    if (MENU_activeMenu->DrawStatusBar)
    {
        MENU_activeMenu->DrawStatusBar();
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
    // MENU_activeMenu = NULL;
    EVHD_subscribeEvent(SERVICE_MENU_EventHandler);
    IsUpdateNeeded = true;
}

void MENU_Denit()
{
    // MENU_activeMenu = NULL;
    EVHD_unsubscriveEvent(SERVICE_MENU_EventHandler);
}

void MENU_Draw()
{
    if (!IsUpdateNeeded)
    {
        return;
    }

    MENU__DrawStatusBar();

    if (MENU_activeMenu)
    {
        MENU_activeMenu->Draw();
    }
    IsUpdateNeeded = false;
}

void MENU_SetActiveMenu(tstMENU_menu* menu, bool deinitOld)
{
    if (deinitOld)
    {
        MENU_activeMenu->Deinit();
    }
    MENU_activeMenu = menu;
    MENU_Update();
}

tstMENU_menu* MENU_GetActiveMenu()
{
    return MENU_activeMenu;
}

void MENU_StickAction()
{
    tstStick stick = STICK_GetStick();
    if (stick.Y.direction == -1 && stick.Y.state == Move
        && MENU_activeMenu->ScrollDown)
    {
        MENU_activeMenu->ScrollDown();
    }
    else if (stick.Y.direction == 1 && stick.Y.state == Move
            && MENU_activeMenu->ScrollUp)
    {
        MENU_activeMenu->ScrollUp();
    }
    if (stick.X.direction == -1 && stick.X.state == Move
        && MENU_activeMenu->ScrollLeft)
    {
        MENU_activeMenu->ScrollLeft();
    }
    else if (stick.X.direction == 1 && stick.X.state == Move
            && MENU_activeMenu->ScrollRight)
    {
        MENU_activeMenu->ScrollRight();
    }
}

void MENU_ClickAction()
{
    // tstStick stick = STICK_GetStick();
}

void SERVICE_MENU_EventHandler(tenEVHD_Event ev)
{
    if (!MENU_activeMenu)
    {
        return;
    }

    if (MENU_activeMenu->preEventHandler)
    {
        MENU_activeMenu->preEventHandler(ev);
    }

    if (ev == EVHD_STICK_YStateChanged)
    {
        MENU_StickAction();
    }
    else if (ev == EVHD_STICK_SWStateChanged)
    {
        tstStick stick = STICK_GetStick();
        if (stick.SW == 1 && MENU_activeMenu->selectedItem && MENU_activeMenu->selectedItem->onClick)
        {

            MENU_activeMenu->selectedItem->onClick();
        }
    }

    if (MENU_activeMenu->postEventHandler)
    {
        MENU_activeMenu->postEventHandler(ev);
    }

}

void MENU_ScrollDown(tstMENU_menu* menu, tstMENU_ListItem* list)
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

void MENU_ScrollUp(tstMENU_menu* menu, tstMENU_ListItem* list)
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
    MENU_SetActiveMenu((tstMENU_menu*)MENU_activeMenu->parent, true);
}

void MENU_SetParent(tstMENU_menu* parent, tstMENU_menu* child)
{
    child->parent = (struct tstMENU_menu*) parent;
}

bool MENU_IsUpdateNeeded()
{
    return IsUpdateNeeded;
}

void MENU_Update()
{
    IsUpdateNeeded = true;
}