/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "MENU.h"

/*
 * The module handles UI for main menu
*/

void MAIN_MENU_Init();
void MAIN_MENU_Deinit();
void MAIN_MENU_Draw();
void MAIN_MENU_ScrollUp();
void MAIN_MENU_ScrollDown();

MENU_tstMenu* MAIN_MENU_GetMenu();

#endif // MAIN_MENU_H
