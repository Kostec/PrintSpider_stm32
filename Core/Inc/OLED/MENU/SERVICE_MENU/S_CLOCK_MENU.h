/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef S_CLOCK_MENU_H
#define S_CLOCK_MENU_H

#include "OLED/MENU/MENU.h"

/*
 * The module handles UI for service clock menu
*/

void S_CLOCK_MENU_Init(MENU_tstMenu* parent);
void S_CLOCK_MENU_Deinit();
void S_CLOCK_MENU_Draw();
void S_CLOCK_MENU_ScrollUp();
void S_CLOCK_MENU_ScrollDown();
void S_CLOCK_MENU_ScrollLeft();
void S_CLOCK_MENU_ScrollRight();
MENU_tstMenu* S_CLOCK_MENU_GetMenu();

#endif // S_CLOCK_MENU_H
