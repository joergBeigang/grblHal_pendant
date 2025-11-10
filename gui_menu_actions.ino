

#include "gui_build.h"

// **********************
// actions for main menu items
// **********************
void actionOff(){
  for (int i = 0; i<rootMenuCount; i++){
    MenuItem &item = rootMenu[i];
    item.selected = (i == cursorPosition);
  }
  drawScreen(cursorPosition);
}

void actionMenu() {}
void actionX() {}
void actionY() {}
void actionZ() {}
void actionJoy() {}
void actionXPos() {}
void actionyPos() {}
void actionzPos() {}
