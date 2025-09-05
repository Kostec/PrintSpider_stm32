#include "OLED/MENU/SERVICE_MENU.h"
#include "DIO/STICK.h"
#include "EVHD/EVHD.h"
#include "stdio.h"

#include "OLED/MENU/SERVICE_MENU/SERVICE_IO_MENU.h"
#include "OLED/MENU/SERVICE_MENU/S_SD_MENU.h"
#include "OLED/MENU/SERVICE_MENU/S_CLOCK_MENU.h"

void SERVICE_MENU__openSD();
void SERVICE_MENU__openIO();
void SERVICE_MENU__openClock();

static tstMENU_ListItem SERVICE_MENU_listItems []= 
{
    {
        .text = "../",
        .onClick = MENU_Exit
    },
    {
        .text = "IO",
        .onClick = SERVICE_MENU__openIO
    },
    {
        .text = "SD",
        .onClick = SERVICE_MENU__openSD
    },
    {
        .text = "Date & Time",
        .onClick = SERVICE_MENU__openClock
    },
};

static tstMENU_menu SERVICE_MENU = {
    .Init = SERVICE_MENU_Init,
    .Deinit = SERVICE_MENU_Deinit,
    .Draw = SERVICE_MENU_Draw,
    .ScrollUp = SERVICE_MENU_ScrollUp,
    .ScrollDown = SERVICE_MENU_ScrollDown,
    .listItemSize = sizeof(SERVICE_MENU_listItems)/sizeof(tstMENU_ListItem)
};

void SERVICE_MENU__openClock()
{
    S_CLOCK_MENU_Init(&SERVICE_MENU);
    MENU_SetActiveMenu(S_CLOCK_MENU_GetMenu(), false);
}

void SERVICE_MENU__openSD()
{
    S_SD_MENU_Init(&SERVICE_MENU);
    MENU_SetActiveMenu(S_SD_MENU_GetMenu(), false);
}

void SERVICE_MENU__openIO()
{
    S_IO_MENU_Init(&SERVICE_MENU);
    MENU_SetActiveMenu(S_IO_MENU_GetMenu(), false);
}

void SERVICE_MENU_drawListItem(tstMENU_ListItem listItem, uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(listItem.text, Font_7x10, listItem.isSelected ? Black : White);
}

void SERVICE_MENU_List()
{
    uint8_t startLine = 16;
    for(uint8_t i = 0; 
        (i < sizeof(SERVICE_MENU_listItems)/sizeof(SERVICE_MENU_listItems[0])) || (startLine + i*10 > 128);
        i++)
    {
        SERVICE_MENU_drawListItem(SERVICE_MENU_listItems[i], 0, 16 + i*10);
    }
    
}

void SERVICE_MENU_ScrollUp()
{
    MENU_ScrollUp(&SERVICE_MENU, SERVICE_MENU_listItems);
}

void SERVICE_MENU_ScrollDown()
{
    MENU_ScrollDown(&SERVICE_MENU, SERVICE_MENU_listItems);
}

void SERVICE_MENU_Init(tstMENU_menu* parent)
{
    SERVICE_MENU.parent = (struct tstMENU_menu*) parent;
    SERVICE_MENU.selectedItemIdx = 0;
    SERVICE_MENU.selectedItem = &SERVICE_MENU_listItems[0];
    SERVICE_MENU.selectedItem->isSelected = true;
}

void SERVICE_MENU_Deinit()
{
    SERVICE_MENU.parent = NULL;
}

void SERVICE_MENU_Draw()
{
    SERVICE_MENU_List();
}

tstMENU_menu* SERVICE_MENU_GetMenu()
{
    return &SERVICE_MENU;
}
