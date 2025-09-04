#ifndef SERVICE_IO_MENU_H
#define SERVICE_IO_MENU_H

#include "OLED/MENU/MENU.h"

void S_IO_MENU_Init(tstMENU_menu* parent);
void S_IO_MENU_Deinit();
void S_IO_MENU_Draw();
void S_IO_MENU_ScrollUp();
void S_IO_MENU_ScrollDown();
tstMENU_menu* S_IO_MENU_GetMenu();

#endif // SERVICE_IO_MENU_H
