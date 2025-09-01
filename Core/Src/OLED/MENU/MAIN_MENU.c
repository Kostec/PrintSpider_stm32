#include "MAIN_MENU.h"
#include "MENU_RESOURCES.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"
#include "DIO/STICK.h"
#include "EVHD/EVHD.h"
#include "stdio.h"

#include "SERVICE_MENU.h"
#include "SD_MENU.h"

void MAIN_MENU_openServiceMenu();
void MAIN_MENU_openSdMenu();

void MAIN_MENU_STICK_STATE();
void MAIN_MENU_StickAction();
void MAIN_MENU_EventHandler(tenEVHD_Event ev);

static tstMENU_ListItem MENU_mainListItems []= 
{
    {
        .text = "Print",
    },
    {
        .text = "File system",
        .onClick = MAIN_MENU_openSdMenu
    },
    {
        .text = "Service mode",
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
static tstMENU_menu MAIN_MENU = {
    .Init = MAIN_MENU_Init,
    .Deinit = MAIN_MENU_Deinit,
    .Draw = MAIN_MENU_Draw,
    .ScrollUp = MAIN_MENU_ScrollUp,
    .ScrollDown = MAIN_MENU_ScrollDown,
    .listItemSize = sizeof(MENU_mainListItems)/sizeof(tstMENU_ListItem)
};

void MAIN_MENU_SD_CARD(bool show)
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

void MAIN_MENU_USB(bool show)
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

void MAIN_MENU_StatusBar()
{
    MAIN_MENU_STICK_STATE();
    MAIN_MENU_SD_CARD(true);
    MAIN_MENU_USB(true);
}

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


void MAIN_MENU_STICK_STATE()
{
    ssd1306_SetCursor(0, 0);
    char stickState[15] = {0};
    tstStick stick = STICK_GetStick();
    sprintf(stickState, "%d %d %u", stick.X.state, stick.Y.state, stick.SW);
    ssd1306_WriteString(stickState, Font_7x10, White);
}

void MAIN_MENU_drawListItem(tstMENU_ListItem listItem, uint8_t x, uint8_t y)
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
    MAIN_MENU_StatusBar();
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

tstMENU_menu* MAIN_MENU_GetMenu()
{
    return &MAIN_MENU;
}

