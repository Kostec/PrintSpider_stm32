#include "MAIN_MENU.h"
#include "MENU_RESOURCES.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"
#include "DIO/STICK.h"
#include "EVHD/EVHD.h"

bool showSDCard = true;
int selectedListItem = -1;

void MAIN_MENU_StickAction();
void MAIN_MENU_EventHandler(tenEVHD_Event ev);

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

void MAIN_MENU_StatusBar()
{
    MAIN_MENU_SD_CARD(true);
}

tstMENU_ListItem MENU_mainListItems []= 
{
    {
        .text = "Print",
    },
    {
        .text = "File system"
    },
    {
        .text = "Service mode"
    },
    {
        .text = "About"
    },
    {
        .text = "Restart"
    }
};

void MAIN_MENU_drawListItem(tstMENU_ListItem listItem, uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(listItem.text, Font_7x10, listItem.isSelected ? Black : White);
}

void MAIN_MENU_List()
{
    uint8_t startLine = 16;
    for(uint8_t i = 0; 
        (i < sizeof(MENU_mainListItems)/sizeof(MENU_mainListItems[0])) || (startLine + i*10 > 128);
        i++)
    {
        MAIN_MENU_drawListItem(MENU_mainListItems[i], 0, 16 + i*10);
    }
    
}

void MAIN_MENU_Init()
{
    EVHD_subscribeEvent(MAIN_MENU_EventHandler);
}

void MAIN_MENU_Draw()
{
    MAIN_MENU_StatusBar();
    MAIN_MENU_StickAction();
    MAIN_MENU_List();
}

void MAIN_MENU_ScrollUp()
{
    if (selectedListItem - 1 >= 0)
    {
        MENU_mainListItems[selectedListItem].isSelected = false;
        selectedListItem--;
        MENU_mainListItems[selectedListItem].isSelected = true;
    }
}

void MAIN_MENU_ScrollDown()
{
    if (selectedListItem + 1 < sizeof(MENU_mainListItems)/sizeof(MENU_mainListItems[0]) -1)
    {
        MENU_mainListItems[selectedListItem].isSelected = false;
        selectedListItem++;
        MENU_mainListItems[selectedListItem].isSelected = true;
    }
}

void MAIN_MENU_StickAction()
{
    tstStick stick = STICK_GetStick();
    if (stick.Y.direction == -1)
    {
        MAIN_MENU_ScrollDown();
    }
    else if (stick.Y.direction == 1)
    {
        MAIN_MENU_ScrollUp();
    }
}

void MAIN_MENU_EventHandler(tenEVHD_Event ev)
{
    if (ev == EVHD_STICK_StateChanged)
    {
        MAIN_MENU_StickAction();
    }

}
