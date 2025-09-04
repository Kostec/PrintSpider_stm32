#include "MENU/SERVICE_MENU/S_SD_MENU.h"
#include "SD/SD.h"
#include "stdio.h"

// static char lines[16][32] = {0};
// static uint8_t startLine = 0;
// static uint8_t linesCount = 0;

static tstMENU_ListItem S_SD_MENU_listItems []= 
{
    {
        .text = "../",
        .onClick = MENU_Exit
    }
};

static tstMENU_menu S_SD_MENU = {
    .Init = S_SD_MENU_Init,
    .Deinit = S_SD_MENU_Deinit,
    .Draw = S_SD_MENU_Draw,
    .ScrollUp = S_SD_MENU_ScrollUp,
    .ScrollDown = S_SD_MENU_ScrollDown,
    .listItemSize = sizeof(S_SD_MENU_listItems)/sizeof(tstMENU_ListItem)
};

void S_SD_MENU_Init(tstMENU_menu* parent)
{
    S_SD_MENU.parent = (struct tstMENU_menu*) parent;
    S_SD_MENU.selectedItemIdx = 0;
    S_SD_MENU.selectedItem = &S_SD_MENU_listItems[0];
    S_SD_MENU.selectedItem->isSelected = true;

    // startLine = 0;
    // linesCount = 0;
}

void S_SD_MENU_Deinit()
{
    S_SD_MENU.parent = NULL;
}

void S_SD_MENU_Draw()
{
    ssd1306_SetCursor(0, 16);
    tstSD_info sdInfo = SD_info("0:/");
    char tmp[32] = {0};
    sprintf(tmp, "Total: %lu", sdInfo.totalSize);
    ssd1306_WriteString(tmp, Font_7x10, White);
    ssd1306_SetCursor(0, 26);
    sprintf(tmp, "Free: %lu", sdInfo.freeSpace);
    ssd1306_WriteString(tmp, Font_7x10, White);
}

void S_SD_MENU_ScrollUp()
{

}

void S_SD_MENU_ScrollDown()
{

}

tstMENU_menu* S_SD_MENU_GetMenu()
{
    return &S_SD_MENU;
}

