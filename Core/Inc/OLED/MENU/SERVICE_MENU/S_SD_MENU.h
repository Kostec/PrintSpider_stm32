#ifndef S_SD_MENU_H
#define S_SD_MENU_H

#include "OLED/MENU/MENU.h"

void S_SD_MENU_Init(tstMENU_menu* parent);
void S_SD_MENU_Deinit();
void S_SD_MENU_Draw();
void S_SD_MENU_ScrollUp();
void S_SD_MENU_ScrollDown();
tstMENU_menu* S_SD_MENU_GetMenu();

#endif // S_SD_MENU_H
