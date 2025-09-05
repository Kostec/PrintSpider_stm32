#ifndef WAIT_H
#define WAIT_H

#include "OLED/MENU/MENU.h"

void WAIT_Init(tstMENU_menu* parent);
void WAIT_Deinit();
void WAIT_Draw();
tstMENU_menu* WAIT_GetMenu();

#endif // WAIT_H