/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef SERVICE_IO_MENU_H
#define SERVICE_IO_MENU_H

#include "OLED/MENU/MENU.h"

/*
 * The module handles UI for service I/O ports menu
*/

void S_IO_MENU_Init(MENU_tstMenu* parent);
void S_IO_MENU_Deinit();
void S_IO_MENU_Draw();
void S_IO_MENU_ScrollUp();
void S_IO_MENU_ScrollDown();
MENU_tstMenu* S_IO_MENU_GetMenu();

#endif // SERVICE_IO_MENU_H
