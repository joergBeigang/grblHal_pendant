/*
 * gui_build.ino
 *
 * definitions of the screens, menu items and actions
 */ 

#include "gui_build.h"

// *********
// root page 
// *********



// build the menu structure
// label, startPosX, startPosY, action, submenu
MenuItem rootMenu[] = {
    {"Menu", 3, 60, nullptr, nullptr},
    {"OFF", 30, 60, nullptr, nullptr},
    {"X", 55, 60, nullptr, nullptr},
    {"Y", 70, 60, nullptr, nullptr},
    {"Z", 85, 60, nullptr, nullptr},
    {"Joy", 100, 60, nullptr, nullptr},
    {"123.123", 3, 20, nullptr, nullptr},
    {"123.123", 26, 20, nullptr, nullptr},
    {"123.123", 55, 20, nullptr, nullptr},
  };
const int rootMenuCount = sizeof(rootMenu) / sizeof(MenuItem);

UiItem rootUi[] = {
    {"X:", u8g2_font_profont12_tr, 0, 13},
    {"Y:", u8g2_font_profont12_tr, 40, 13},
    {"Z:", u8g2_font_profont12_tr, 80, 13},
    {"Feed:", u8g2_font_4x6_tr, 0, 33},
    {"Spindle:", u8g2_font_4x6_tr, 35, 33},
    {"Rapid:",u8g2_font_4x6_tr, 80, 33},
    {"State:", u8g2_font_4x6_tr, 50, 43},
  };
const int rootUiCount = sizeof(rootUi) / sizeof(UiItem);
UiDynamicItem rootDynamicUi[] = {
    {"Feed:", u8g2_font_4x6_tr, 20, 33, &grblStatus.overRides[0], nullptr},
    {"Spindle:", u8g2_font_4x6_tr, 70, 33, &grblStatus.overRides[1], nullptr},
    {"Rapid:",u8g2_font_4x6_tr, 110, 33, &grblStatus.overRides[2], nullptr},
    {"State:", u8g2_font_4x6_tr, 80, 43, nullptr, &grblStatus.status},
  };

const int rootDynamicUiCount = sizeof(rootDynamicUi) / sizeof(UiDynamicItem);

UiPage rootPage = {
  "Root",
  rootMenu,
  rootMenuCount,
  rootUi,
  rootUiCount,
  rootDynamicUi,
  rootDynamicUiCount
};




// **********************
// actions for metu items
// **********************
void actionOff(){
  for (int i = 0; i<rootMenuCount; i++){
    MenuItem &item = rootMenu[i];
    if (i == cursorPosition){
      Serial.print(i);
      Serial.println("Action OFF");
      item.selected = true;
    }
    else {
      item.selected = false;
    }
  }
  drawMainScreen(cursorPosition);  // redraw cursor
}


