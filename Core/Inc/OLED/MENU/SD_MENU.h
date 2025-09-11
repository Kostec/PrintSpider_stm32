/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef SD_MENU_H
#define SD_MENU_H

#include "MENU.h"

/*
 * The module handles UI for sd menu
*/

void SD_MENU_Init(MENU_tstMenu* parent);
void SD_MENU_Deinit();
void SD_MENU_Draw();
void SD_MENU_ScrollUp();
void SD_MENU_ScrollDown();

MENU_tstMenu* SD_MENU_GetMenu();

#endif // SD_MENU_H
