#ifndef SERVICE_MENU_H
#define SERVICE_MENU_H

#include "OLED/MENU/MENU.h"

void SERVICE_MENU_Init(tstMENU_menu* parent);
void SERVICE_MENU_Deinit();
void SERVICE_MENU_Draw();
void SERVICE_MENU_ScrollUp();
void SERVICE_MENU_ScrollDown();
tstMENU_menu* SERVICE_MENU_GetMenu();

#endif // SERVICE_MENU_H
