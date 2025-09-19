/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/SD_MENU.h"
#include "SD/SD.h"
#include "string.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"

#define SD_MAX_TMP_COUNT (10)
#define SD_MAX_TextString (32)
#define SD_RootDir "0:/"

static MENU_tstListItem SD_listItems [SD_MAX_TMP_COUNT]= {0};
static char SD_rawList[SD_MAX_TMP_COUNT][SD_MAX_TextString] = {0};
static char SD_ReadDir[64] = SD_RootDir;
static int SD_DirItemsCount = 0;
static bool SD_Readed = false;
static bool SD_WaitClb = false;
static uint8_t SD_startRead = 0;
static uint8_t SD_endRead = 0;
const static uint8_t SD_readStep = 8;

static MENU_tstMenu SD_MENU =
{
    .Init = SD_MENU_Init,
    .Deinit = SD_MENU_Deinit,
    .Draw = SD_MENU_Draw,
    .ScrollUp = SD_MENU_ScrollUp,
    .ScrollDown = SD_MENU_ScrollDown,
    .listItemSize = sizeof(SD_listItems)/sizeof(MENU_tstListItem)
};

void SD_MENU__DirItemsCount(int res)
{
    SD_DirItemsCount = res;
    MENU_SetWait(false);
}

void SD_MENU__IsDirOnClick(bool res)
{
    if (res)
    {
        strcat(SD_ReadDir, SD_MENU.selectedItem->text);
        SD_Readed = false;
        SD_MENU.selectedItem->isSelected = false;
        SD_MENU.selectedItemIdx = 0;
        SD_MENU.selectedItem = &SD_listItems[0];
        SD_MENU.selectedItem->isSelected = true;
        SD_dirItemsCount(SD_MENU__DirItemsCount, SD_ReadDir);
    }
    else
    {
        MENU_SetWait(false);
    }
}

void SD_MENU__DirUpOnClick(bool res)
{
    if(res)
    {
        SD_Readed = false;
    }
    MENU_SetWait(false);
}

void SD_MENU__OnClick()
{
    char path[64] = {0};

    if (SD_MENU.selectedItemIdx == 0)
    {
        if (!strcmp(SD_ReadDir, SD_RootDir) || !strcmp(SD_ReadDir, "/"))
        {
            MENU_Exit();
            return;
        }
        else 
        {
            SD_dirUp(SD_MENU__DirUpOnClick, SD_ReadDir);
            MENU_SetWait(true);
            return;
        }
        
    }

    strcpy(path, SD_ReadDir);
    strcat(path, SD_MENU.selectedItem->text);
    MENU_SetWait(true);
    SD_isDir(SD_MENU__IsDirOnClick, SD_MENU.selectedItem->text);
}

void SD_MENU_Init(MENU_tstMenu* parent)
{
    SD_MENU.parent = (struct MENU_tstMenu*) parent;
    SD_startRead = 0;
    SD_DirItemsCount = 0;
    SD_endRead = SD_startRead + SD_readStep;
    SD_Readed = false;
    SD_WaitClb = false;
    SD_MENU.selectedItemIdx = 0;
    SD_MENU.selectedItem = &SD_listItems[0];
    memset(SD_ReadDir, 0, sizeof(SD_ReadDir));
    strcat(SD_ReadDir, SD_RootDir);
    for(int i = 0; i < sizeof(SD_listItems)/sizeof(SD_listItems[0]); i++)
    {
        SD_listItems[i].onClick = SD_MENU__OnClick;
    }
    SD_MENU.selectedItem->isSelected = true;
    SD_MENU.selectedItem->text = "../";

    MENU_SetWait(true);
    SD_dirItemsCount(SD_MENU__DirItemsCount, SD_ReadDir);
}

void SD_MENU_Deinit()
{
    SD_MENU.parent = NULL;
}

static void SD__ReadDirCallback(uint8_t idx, char* str, size_t len)
{
    memcpy(SD_rawList[idx+1], str, len); // 1 is reserved for ../
    MENU_SetWait(false);
}

void SD_MENU_drawListItem(MENU_tstListItem listItem, uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(listItem.text, Font_7x10, listItem.isSelected ? Black : White);
}

void SD_MENU_List()
{
    uint8_t startLine = 16;
    uint8_t startItemIdx = SD_MENU.selectedItemIdx > 1 ? SD_MENU.selectedItemIdx - 1 : 0;
    for(uint8_t i = 0; 
        (startItemIdx + i < SD_MENU.listItemSize) && (startLine + i*20 < 128);
        i++)
    {
        if (SD_MENU.selectedItemIdx == startItemIdx + i)
        {
            SD_listItems[startItemIdx + i].isSelected = true;
            SD_MENU.selectedItem = &SD_listItems[startItemIdx + i];
        }
        else
        {
            SD_listItems[startItemIdx + i].isSelected = false;
        }

        SD_MENU_drawListItem(SD_listItems[startItemIdx + i], 0, 16 + i*10);
    }
    
}

void SD_MENU__ReadDirClb(int res)
{
    SD_MENU.listItemSize = res + 1;

    if (SD_MENU.listItemSize == -1)
    {
        SD_Readed = false;
    }
    SD_Readed = true;

    SD_MENU.selectedItemIdx = 0;
    SD_MENU.selectedItem = &SD_listItems[0];
    SD_MENU.selectedItem->isSelected = true;
    for(uint8_t i = 1; i < SD_MENU.listItemSize; i++)
    {
        SD_listItems[i].text = (char*)SD_rawList[i];
        SD_listItems[i].isSelected = false;
    }

    SD_WaitClb = false;
    MENU_SetWait(false);
}

void SD_MENU_Draw()
{
    if (!SD_isAvailable())
    {
        ssd1306_SetCursor(0, 16);
        ssd1306_WriteString("SD is not available!", Font_7x10, White);
        return;
    }
    
    if (!SD_Readed && !SD_WaitClb)
    {
        memset(SD_rawList[1], 0, (SD_MAX_TMP_COUNT-1)*SD_MAX_TextString);
        MENU_SetWait(true);
        SD_readDir(SD_MENU__ReadDirClb, SD__ReadDirCallback, SD_ReadDir, SD_startRead, SD_endRead);
        SD_WaitClb = true;
    }

    SD_MENU_List();
}

void SD_MENU_ScrollUp()
{
    if (SD_MENU.selectedItemIdx == 0 && SD_startRead >= SD_readStep)
    {
        SD_startRead -= SD_readStep;
        SD_endRead = SD_startRead + SD_readStep;
        SD_Readed = false;
    }
    else
    {
        MENU_ScrollUp(&SD_MENU, SD_listItems);
    }
}

void SD_MENU_ScrollDown()
{
    MENU_ScrollDown(&SD_MENU, SD_listItems);
    if (SD_MENU.selectedItemIdx == SD_MENU.listItemSize-1 && SD_startRead+SD_readStep < SD_DirItemsCount)
    {
        SD_startRead += SD_readStep;
        SD_endRead = SD_startRead + SD_readStep;
        SD_Readed = false;
    }
}


MENU_tstMenu* SD_MENU_GetMenu()
{
    return &SD_MENU;
}
