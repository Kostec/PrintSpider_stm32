/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef WAIT_H
#define WAIT_H

#include "OLED/MENU/MENU.h"

/*
 * The module handles UI for wait animation popup
*/

void WAIT_Init(MENU_tstMenu* parent);
void WAIT_Deinit();
void WAIT_Draw();
MENU_tstMenu* WAIT_GetMenu();

#endif // WAIT_H