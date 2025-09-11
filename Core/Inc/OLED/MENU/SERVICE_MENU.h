/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef SERVICE_MENU_H
#define SERVICE_MENU_H

#include "OLED/MENU/MENU.h"

/*
 * The module handles UI for service menu
*/

void SERVICE_MENU_Init(MENU_tstMenu* parent);
void SERVICE_MENU_Deinit();
void SERVICE_MENU_Draw();
void SERVICE_MENU_ScrollUp();
void SERVICE_MENU_ScrollDown();
MENU_tstMenu* SERVICE_MENU_GetMenu();

#endif // SERVICE_MENU_H
