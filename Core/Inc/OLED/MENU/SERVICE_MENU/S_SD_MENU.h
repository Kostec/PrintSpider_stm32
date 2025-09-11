/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef S_SD_MENU_H
#define S_SD_MENU_H

#include "OLED/MENU/MENU.h"

/*
 * The module handles UI for service sd menu
*/

void S_SD_MENU_Init(MENU_tstMenu* parent);
void S_SD_MENU_Deinit();
void S_SD_MENU_Draw();
void S_SD_MENU_ScrollUp();
void S_SD_MENU_ScrollDown();
MENU_tstMenu* S_SD_MENU_GetMenu();

#endif // S_SD_MENU_H
