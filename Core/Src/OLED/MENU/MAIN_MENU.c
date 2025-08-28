#include "MAIN_MENU.h"
#include "MENU_RESOURCES.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"
#include "DIO/STICK.h"

bool showSDCard = true;
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
        .isSelected = true
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
    // if (listItem.isSelected)
    // {
    //     ssd1306_InvertRectangle(x, y, 128, y + 10);
    // }
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

void MAIN_MENU_Draw()
{
    MAIN_MENU_StatusBar();
    MAIN_MENU_List();
}

void MAIN_MENU_Action()
{

}