#include "MENU/MENU.h"
#include "DIO/STICK.h"


static tstMENU_menu* MENU_activeMenu;
void SERVICE_MENU_EventHandler(tenEVHD_Event ev);

void MENU_Init()
{
    // MENU_activeMenu = NULL;
    EVHD_subscribeEvent(SERVICE_MENU_EventHandler);
}

void MENU_Denit()
{
    // MENU_activeMenu = NULL;
    EVHD_unsubscriveEvent(SERVICE_MENU_EventHandler);
}

void MENU_Draw()
{
    if (MENU_activeMenu)
    {
        MENU_activeMenu->Draw();
    }
}

void MENU_SetActiveMenu(tstMENU_menu* menu, bool deinitOld)
{
    if (deinitOld)
    {
        MENU_activeMenu->Deinit();
    }
    MENU_activeMenu = menu;
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
}

void MENU_Exit()
{
    MENU_SetActiveMenu((tstMENU_menu*)MENU_activeMenu->parent, true);
}

void MENU_SetParent(tstMENU_menu* parent, tstMENU_menu* child)
{
    child->parent = (struct tstMENU_menu*) parent;
}