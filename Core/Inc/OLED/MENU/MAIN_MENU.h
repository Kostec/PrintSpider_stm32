#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <stdbool.h>

typedef struct
{
    char* text;
    bool isSelected;
    void (*onClick)();
    void (*onDoubleClick)();
    void (*onPress)();
} tstMENU_ListItem;

void MAIN_MENU_Draw();

#endif // MAIN_MENU_H
