#ifndef S_CLOCK_MENU_H
#define S_CLOCK_MENU_H

#include "OLED/MENU/MENU.h"

void S_CLOCK_MENU_Init(tstMENU_menu* parent);
void S_CLOCK_MENU_Deinit();
void S_CLOCK_MENU_Draw();
void S_CLOCK_MENU_ScrollUp();
void S_CLOCK_MENU_ScrollDown();
void S_CLOCK_MENU_ScrollLeft();
void S_CLOCK_MENU_ScrollRight();
tstMENU_menu* S_CLOCK_MENU_GetMenu();

#endif // S_CLOCK_MENU_H
