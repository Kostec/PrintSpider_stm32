#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "MENU.h"

void MAIN_MENU_Init();
void MAIN_MENU_Deinit();
void MAIN_MENU_Draw();
void MAIN_MENU_ScrollUp();
void MAIN_MENU_ScrollDown();

tstMENU_menu* MAIN_MENU_GetMenu();

#endif // MAIN_MENU_H
