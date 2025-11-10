/*
 * gui_build.ino
 *
 * definitions of the screens, menu items and actions
 */ 

#include "gui_build.h"
#include "gui_menu_actions.h"

// *********
// root page 
// *********



// build the menu structure
// label, startPosX, startPosY, action, submenu

MenuItem rootMenu[] = {
    {.label = "Menu", .x = 3, .y = 60, .action = actionMenu, .submenu = nullptr},
    {.label = "OFF",  .x = 30, .y = 60, .action = actionOff,  .submenu = nullptr},
    {.label = "X",    .x = 55, .y = 60, .action = actionX,    .submenu = nullptr},
    {.label = "Y",    .x = 70, .y = 60, .action = actionY,    .submenu = nullptr},
    {.label = "Z",    .x = 85, .y = 60, .action = actionZ,    .submenu = nullptr},
    {.label = "Joy",  .x = 100,.y = 60, .action = actionJoy,  .submenu = nullptr},
    {.label = "123.123", .x = 3,  .y = 20, .action = actionXPos, .submenu = nullptr, .value = &grblStatus.position[0]},
    {.label = "123.123", .x = 55, .y = 20, .action = actionyPos, .submenu = nullptr, .value = &grblStatus.position[1]},
    {.label = "123.123", .x = 80, .y = 20, .action = actionzPos, .submenu = nullptr, .value = &grblStatus.position[2]},
};
// MenuItem rootMenu[] = {
//     {.label = "Menu", .x = 3, .y = 60, .action = actionMenu},
//     {.label = "OFF", .x = 30, .y = 60, .action = actionOff},
//     {.label = "X", .x = 55, .y = 60, .action = actionX},
//     {.label = "Y", .x = 70, .y = 60, .action = actionY},
//     {.label = "Z", .x = 85, .y = 60, .action = actionZ},
//     {.label = "Joy", .x = 100, .y = 60, .action = actionJoy},
//     { .label = "123.123", .x = 3, .y = 20, .value = &grblStatus.position[0], .action = actionXPos },
//     { .label = "123.123", .x = 55, .y = 20, .value = &grblStatus.position[1], .action = actionyPos },
//     { .label = "123.123", .x = 80, .y = 20, .value = &grblStatus.position[2], .action = actionzPos },
// };
const int rootMenuCount = sizeof(rootMenu) / sizeof(MenuItem);

UiItem rootUi[] = {
    {.label = "X:", .font = u8g2_font_profont12_tr, .x = 0, .y = 13},
    {.label = "Y:", .font = u8g2_font_profont12_tr, .x = 40, .y = 13},
    {.label = "Z:", .font = u8g2_font_profont12_tr, .x = 80, .y = 13},
    {.label = "Feed:", .font = u8g2_font_4x6_tr, .x = 0, .y = 33},
    {.label = "Spindle:", .font = u8g2_font_4x6_tr, .x = 35, .y = 33},
    {.label = "Rapid:", .font = u8g2_font_4x6_tr, .x = 80, .y = 33},
    {.label = "State:", .font = u8g2_font_4x6_tr, .x = 50, .y = 43},
  };
const int rootUiCount = sizeof(rootUi) / sizeof(UiItem);
UiDynamicItem rootDynamicUi[] = {
    {"Feed:", u8g2_font_4x6_tr, 20, 33, &grblStatus.overRides[0], nullptr,0},
    {"Spindle:", u8g2_font_4x6_tr, 70, 33, &grblStatus.overRides[1], nullptr,0},
    {"Rapid:",u8g2_font_4x6_tr, 110, 33, &grblStatus.overRides[2], nullptr,0},
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

//
// *****************************************************
// currwent page pointer is what is going to be rendered
// *****************************************************
UiPage* currentPage = &rootPage;


