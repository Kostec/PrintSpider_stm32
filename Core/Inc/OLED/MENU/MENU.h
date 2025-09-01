#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "MENU_RESOURCES.h"
#include "EVHD/EVHD.h"

typedef struct
{
    char* text;
    bool isSelected;
    void (*onClick)();
    void (*onDoubleClick)();
    void (*onPress)();
} tstMENU_ListItem;

typedef struct 
{   
    struct tstMENU_menu* parent;
    tstMENU_ListItem* selectedItem;
    int selectedItemIdx;
    uint8_t listItemSize;
    void (*Init)();
    void (*Deinit)();
    void (*Draw)();
    void (*ScrollDown)();
    void (*ScrollUp)(); 
    void (*ScrollRight)();
    void (*ScrollLeft)(); 
    void (*preEventHandler)(tenEVHD_Event ev);
    void (*postEventHandler)(tenEVHD_Event ev);
} tstMENU_menu;

void MENU_Init();
void MENU_Denit();
void MENU_Draw();
void MENU_SetActiveMenu(tstMENU_menu* menu, bool deinitOld);
tstMENU_menu* MENU_GetActiveMenu();
void MENU_ScrollDown(tstMENU_menu* menu, tstMENU_ListItem* list);
void MENU_ScrollUp(tstMENU_menu* menu, tstMENU_ListItem* list);
void MENU_Exit();
void MENU_SetParent(tstMENU_menu* parent, tstMENU_menu* child);


#endif // MENU_H