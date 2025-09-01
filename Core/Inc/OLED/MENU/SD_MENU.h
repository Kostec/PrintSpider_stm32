#ifndef SD_MENU_H
#define SD_MENU_H

#include "MENU.h"

void SD_MENU_Init(tstMENU_menu* parent);
void SD_MENU_Deinit();
void SD_MENU_Draw();
void SD_MENU_ScrollUp();
void SD_MENU_ScrollDown();

tstMENU_menu* SD_MENU_GetMenu();

#endif // SD_MENU_H
