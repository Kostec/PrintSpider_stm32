/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "OLED/MENU/SERVICE_MENU/S_CLOCK_MENU.h"
#include <stdio.h>
#include <string.h>

typedef enum
{
    S_CLOCK_MENU_stateList,
    S_CLOCK_MENU_stateDateDate,
    S_CLOCK_MENU_stateDateMonth,
    S_CLOCK_MENU_stateDateYear,
    S_CLOCK_MENU_stateTimeHours,
    S_CLOCK_MENU_stateTimeMinutes,
    S_CLOCK_MENU_stateTimeSeconds,
} S_CLOCK_MENU_State;

extern RTC_HandleTypeDef hrtc;

static RTC_TimeTypeDef S_CLOCK_time;
static RTC_DateTypeDef S_CLOCK_date;
static S_CLOCK_MENU_State S_CLOCK_state;

void S_CLOCK__onTimeClick();
void S_CLOCK__onDateClick();

static MENU_tstListItem S_CLOCK_listItems [] =
{
    {
        .text = "..",
        .onClick = MENU_Exit
    },
    {
        .text = "", // Time
        .onClick = S_CLOCK__onTimeClick
    },
    {
        .text = "", // Date
        .onClick = S_CLOCK__onDateClick
    }
};

static MENU_tstMenu S_CLOCK_MENU =
{
    .Init = S_CLOCK_MENU_Init,
    .Deinit = S_CLOCK_MENU_Deinit,
    .Draw = S_CLOCK_MENU_Draw,
    .ScrollUp = S_CLOCK_MENU_ScrollUp,
    .ScrollDown = S_CLOCK_MENU_ScrollDown,
    .ScrollLeft = S_CLOCK_MENU_ScrollLeft,
    .ScrollRight = S_CLOCK_MENU_ScrollRight,
    .listItemSize = sizeof(S_CLOCK_listItems)/sizeof(MENU_tstListItem)
};

void S_CLOCK__onTimeClick()
{
    S_CLOCK_state = S_CLOCK_state == S_CLOCK_MENU_stateList ? S_CLOCK_MENU_stateTimeHours : S_CLOCK_MENU_stateList;
    if (S_CLOCK_state == S_CLOCK_MENU_stateList)
    {
        HAL_RTC_SetTime(&hrtc, &S_CLOCK_time, RTC_FORMAT_BIN);
    }
}

void S_CLOCK__onDateClick()
{
    S_CLOCK_state = S_CLOCK_state == S_CLOCK_MENU_stateList ? S_CLOCK_MENU_stateDateDate : S_CLOCK_MENU_stateList;
    if (S_CLOCK_state == S_CLOCK_MENU_stateList)
    {
        HAL_RTC_SetDate(&hrtc, &S_CLOCK_date, RTC_FORMAT_BIN);
    }
}

void S_CLOCK_MENU_Init(MENU_tstMenu* parent)
{
    S_CLOCK_MENU.parent = (struct MENU_tstMenu*) parent;
    for(uint8_t i = 0; i < sizeof(S_CLOCK_listItems)/sizeof(MENU_tstListItem); i++)
    {
        S_CLOCK_listItems->isSelected = false;
    }

    S_CLOCK_MENU.selectedItemIdx = 0;
    S_CLOCK_MENU.selectedItem = &S_CLOCK_listItems[0];
    S_CLOCK_MENU.selectedItem->isSelected = true;

    HAL_RTC_GetTime(&hrtc, &S_CLOCK_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &S_CLOCK_date, RTC_FORMAT_BIN);

    S_CLOCK_state = S_CLOCK_MENU_stateList;
}

void S_CLOCK_MENU_Deinit()
{

}

void S_CLOCK_MENU__drawListItem(MENU_tstListItem listItem, uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(listItem.text, Font_7x10, listItem.isSelected ? Black : White);
}

void S_CLOCK_MENU__DrawList()
{
    uint8_t startLine = 16;
    for(uint8_t i = 0; 
        (i < S_CLOCK_MENU.listItemSize) || (startLine + i*10 > 128);
        i++)
    {
        S_CLOCK_MENU__drawListItem(S_CLOCK_listItems[i], 0, 16 + i*10);
    }
    
}

void S_CLOCK_MENU_Draw()
{
    static char timeStr[24];
    static char dateStr[24];
    memset(timeStr, 0, sizeof(timeStr));
    memset(dateStr, 0, sizeof(dateStr));
    sprintf(timeStr, "Time: %02u:%02u:%02u", S_CLOCK_time.Hours, S_CLOCK_time.Minutes, S_CLOCK_time.Seconds);
    sprintf(dateStr, "Date: %02u/%02u/%02u", S_CLOCK_date.Date, S_CLOCK_date.Month, S_CLOCK_date.Year);
    S_CLOCK_listItems[1].text = timeStr;
    S_CLOCK_listItems[2].text = dateStr;

    S_CLOCK_MENU__DrawList();
}

void S_CLOCK_MENU_ScrollUp()
{
    switch (S_CLOCK_state)
    {
        case S_CLOCK_MENU_stateList: MENU_ScrollUp(S_CLOCK_MENU_GetMenu(), S_CLOCK_listItems); break;
        case S_CLOCK_MENU_stateTimeHours: S_CLOCK_time.Hours = S_CLOCK_time.Hours >= 23 ? 0 : S_CLOCK_time.Hours+1; break;
        case S_CLOCK_MENU_stateTimeMinutes: S_CLOCK_time.Minutes = S_CLOCK_time.Minutes >= 59 ? 0 : S_CLOCK_time.Minutes+1; break;
        case S_CLOCK_MENU_stateTimeSeconds: S_CLOCK_time.Seconds = S_CLOCK_time.Seconds >= 59 ? 0 : S_CLOCK_time.Seconds+1; break;

        case S_CLOCK_MENU_stateDateDate: S_CLOCK_date.Date = S_CLOCK_date.Date >= 31 ? 1 : S_CLOCK_date.Date+1; break;
        case S_CLOCK_MENU_stateDateMonth: S_CLOCK_date.Month = S_CLOCK_date.Month >= 12 ? 1 : S_CLOCK_date.Month+1; break;
        case S_CLOCK_MENU_stateDateYear: S_CLOCK_date.Year = S_CLOCK_date.Year >= 99 ? 0 : S_CLOCK_date.Year+1; break;
    }
}

void S_CLOCK_MENU_ScrollDown()
{
    switch (S_CLOCK_state)
    {
        case S_CLOCK_MENU_stateList: MENU_ScrollDown(S_CLOCK_MENU_GetMenu(), S_CLOCK_listItems); break;
        case S_CLOCK_MENU_stateTimeHours: S_CLOCK_time.Hours = S_CLOCK_time.Hours == 0 ? 23 : S_CLOCK_time.Hours-1; break;
        case S_CLOCK_MENU_stateTimeMinutes: S_CLOCK_time.Minutes = S_CLOCK_time.Minutes == 0 ? 59 : S_CLOCK_time.Minutes-1; break;
        case S_CLOCK_MENU_stateTimeSeconds: S_CLOCK_time.Seconds = S_CLOCK_time.Seconds == 0 ? 59 : S_CLOCK_time.Seconds-1; break;

        case S_CLOCK_MENU_stateDateDate: S_CLOCK_date.Date = S_CLOCK_date.Date <= 1 ? 31 : S_CLOCK_date.Date-1; break;
        case S_CLOCK_MENU_stateDateMonth: S_CLOCK_date.Month = S_CLOCK_date.Month <= 1 ? 12 : S_CLOCK_date.Month-1; break;
        case S_CLOCK_MENU_stateDateYear: S_CLOCK_date.Year = S_CLOCK_date.Year == 0 ? 99 : S_CLOCK_date.Year-1; break;
    }
}

void S_CLOCK_MENU_ScrollLeft()
{
    switch (S_CLOCK_state)
    {
        case S_CLOCK_MENU_stateList: break;
        case S_CLOCK_MENU_stateTimeHours: S_CLOCK_state = S_CLOCK_MENU_stateTimeSeconds; break;
        case S_CLOCK_MENU_stateTimeMinutes: S_CLOCK_state = S_CLOCK_MENU_stateTimeHours; break;
        case S_CLOCK_MENU_stateTimeSeconds: S_CLOCK_state = S_CLOCK_MENU_stateTimeMinutes; break;

        case S_CLOCK_MENU_stateDateDate: S_CLOCK_state = S_CLOCK_MENU_stateDateYear; break;
        case S_CLOCK_MENU_stateDateMonth: S_CLOCK_state = S_CLOCK_MENU_stateDateDate; break;
        case S_CLOCK_MENU_stateDateYear: S_CLOCK_state = S_CLOCK_MENU_stateDateMonth; break;
    }
}

void S_CLOCK_MENU_ScrollRight()
{
    switch (S_CLOCK_state)
    {
        case S_CLOCK_MENU_stateList: break;
        case S_CLOCK_MENU_stateTimeHours: S_CLOCK_state = S_CLOCK_MENU_stateTimeMinutes; break;
        case S_CLOCK_MENU_stateTimeMinutes: S_CLOCK_state = S_CLOCK_MENU_stateTimeSeconds; break;
        case S_CLOCK_MENU_stateTimeSeconds: S_CLOCK_state = S_CLOCK_MENU_stateTimeHours; break;

        case S_CLOCK_MENU_stateDateDate: S_CLOCK_state = S_CLOCK_MENU_stateDateMonth; break;
        case S_CLOCK_MENU_stateDateMonth: S_CLOCK_state = S_CLOCK_MENU_stateDateYear; break;
        case S_CLOCK_MENU_stateDateYear: S_CLOCK_state = S_CLOCK_MENU_stateDateDate; break;
    }
}

MENU_tstMenu* S_CLOCK_MENU_GetMenu()
{
    return &S_CLOCK_MENU;
}
