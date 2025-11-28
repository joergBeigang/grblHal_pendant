/*
 * gui_build.ino
 *
 * definitions of the screens, menu items and actions
 */ 

#include "gui_build.h"
#include "gui_menu_actions.h"

// Dummy empty arrays
UiItem emptyUI[1] = {};
UiDynamicItem emptyDynUI[1] = {};

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
    {.label = "Joystick",  .x = 80,.y = 60, .action = actionJoy,  .submenu = nullptr},
    {.label = "123.123", .x = 1,  .y = 13, .action = actionXPos, .submenu = nullptr, .value = &grblStatus.position[0]},
    {.label = "123.123", .x = 43, .y = 13, .action = actionYPos, .submenu = nullptr, .value = &grblStatus.position[1]},
    {.label = "123.123", .x = 83, .y = 13, .action = actionZPos, .submenu = nullptr, .value = &grblStatus.position[2]},
};
const int rootMenuCount = sizeof(rootMenu) / sizeof(MenuItem);

UiItem rootUi[] = {
    // {.label = "X:", .font = u8g2_font_profont12_tr, .x = 0, .y = 13},
    // {.label = "Y:", .font = u8g2_font_profont12_tr, .x = 40, .y = 13},
    // {.label = "Z:", .font = u8g2_font_profont12_tr, .x = 80, .y = 13},
    {.label = "Feed:", .font = u8g2_font_4x6_tr, .x = 0, .y = 43},
    {.label = "Spindle:", .font = u8g2_font_4x6_tr, .x = 35, .y = 43},
    {.label = "Rapid:", .font = u8g2_font_4x6_tr, .x = 80, .y = 43},
    {.label = "State:", .font = u8g2_font_4x6_tr, .x = 28, .y = 30},
  };
const int rootUiCount = sizeof(rootUi) / sizeof(UiItem);

UiDynamicItem rootDynamicUi[] = {
    {"Feed:", u8g2_font_4x6_tr, 20, 43, &grblStatus.overRides[0], nullptr,0},
    {"Spindle:", u8g2_font_4x6_tr, 70, 43, &grblStatus.overRides[1], nullptr,0},
    {"Rapid:",u8g2_font_4x6_tr, 110, 43, &grblStatus.overRides[2], nullptr,0},
    {"State:",u8g2_font_profont12_tr  , 56, 30, nullptr, &grblStatus.status},
  };

const int rootDynamicUiCount = sizeof(rootDynamicUi) / sizeof(UiDynamicItem);

UiPage rootPage = {
  "Root",
  rootMenu,
  rootMenuCount,
  rootUi,
  rootUiCount,
  rootDynamicUi,
  rootDynamicUiCount,
  nullptr
};

// ********************
// set axis page common
// ********************


MenuItem setAxisMenu[] = {
    {.label = "123.123", .x = 40,  .y = 33, .action = actionSetValue, .submenu = nullptr, .value = pValueEdit},
    // {.label = "0.0", .x = 40, .y = 30, .action = actionSetValue, .submenu = nullptr},
    {.label = "Cancel", .x = 3, .y = 60, .action = actionCancel, .submenu = nullptr},
    {.label = "Set",  .x = 50, .y = 60, .action = actionSetAxisX,  .submenu = nullptr},
};
const int setAxisMenuCount = sizeof(setAxisMenu) / sizeof(MenuItem);

UiItem setAxisXUI[] = {
    {.label = "Set X Axis", .font = u8g2_font_profont12_tr, .x = 30, .y = 15},
};
const int setAxisXUICount = sizeof(setAxisXUI) / sizeof(UiItem);

UiItem setAxisYUI[] = {
    {.label = "Set Y Axis", .font = u8g2_font_profont12_tr, .x = 30, .y = 15},
};
const int setAxisYUICount = sizeof(setAxisYUI) / sizeof(UiItem);

UiItem setAxisZUI[] = {
    {.label = "Set Z Axis", .font = u8g2_font_profont12_tr, .x = 30, .y = 15},
};
const int setAxisZUICount = sizeof(setAxisZUI) / sizeof(UiItem);

// ********************
// set X axis page 
// ********************

UiPage setAxisXPage = {
  "Set Axis",
  setAxisMenu,
  setAxisMenuCount,
  setAxisXUI,
  setAxisXUICount,
  emptyDynUI,
  0,
  &rootPage
};

// ********************
// set Y axis page 
// ********************

UiPage setAxisYPage = {
  "Set Axis",
  setAxisMenu,
  setAxisMenuCount,
  setAxisYUI,
  setAxisYUICount,
  emptyDynUI,
  0,
  &rootPage
};

// ********************
// set Z axis page 
// ********************

UiPage setAxisZPage = {
  "Set Axis",
  setAxisMenu,
  setAxisMenuCount,
  setAxisZUI,
  setAxisZUICount,
  emptyDynUI,
  0,
  &rootPage
};

// const int setAxisMenuCount = sizeof(setAxisMenu) / sizeof(MenuItem);


// **************
// menu 
// **************

MenuItem menuMenu[] = {
    {.label = "Back", .x = 10,  .y = 10, .action = actionCancel, .submenu = nullptr },
    {.label = "Reset", .x = 10,  .y = 25, .action = actionReset, .submenu = nullptr },
    {.label = "Unlock", .x = 10, .y = 50, .action = actionUnlock, .submenu = nullptr},
    {.label = "Homing",  .x = 10, .y = 60, .action = actionHomingMenu,  .submenu = nullptr},
};
const int menuMenuCount = sizeof(menuMenu) / sizeof(MenuItem);

UiPage menuPage = {
  "Menu",
  menuMenu,
  menuMenuCount,
  emptyUI,
  0,
  emptyDynUI,
  0,
  &rootPage
};

// **************
// confirm Homing
// **************

MenuItem confirmHomingMenu[] = {
    {.label = "Cancel", .x = 10,  .y = 50, .action = actionCancel, .submenu = nullptr },
    {.label = "Confirm", .x = 70,  .y = 50, .action = actionHoming, .submenu = nullptr },
};
const int confirmHomingMenuCount = sizeof(confirmHomingMenu) / sizeof(MenuItem);

UiItem confirmHomingUI[] = {
    {.label = "Homeing all axis?", .font = u8g2_font_profont12_tr, .x = 10, .y = 15},
};
const int confirmHomingUICount = sizeof(confirmHomingUI) / sizeof(UiItem);

UiPage confirmHomingPage = {
  "confirm homing",
  confirmHomingMenu,
  confirmHomingMenuCount,
  confirmHomingUI,
  confirmHomingUICount,
  emptyDynUI,
  0,
  &rootPage
};


// ***************
// run code screen
// ***************


MenuItem runMenu[] = {
    {.label = "Pause", .x = 3, .y = 60, .action = actionMenu, .submenu = nullptr},
    {.label = "Resume",  .x = 30, .y = 60, .action = actionOff,  .submenu = nullptr},
};

const int runMenuCount = sizeof(runMenu) / sizeof(MenuItem);

UiDynamicItem runDynamicUi[] = {
    {"123.123", u8g2_font_4x6_tr, 1, 13, &grblStatus.position[0], nullptr, 3},
    {"123.123", u8g2_font_4x6_tr, 43,  13, &grblStatus.position[1], nullptr, 3},
    {"123.123", u8g2_font_4x6_tr, 83,  13, &grblStatus.position[2], nullptr, 3},
    {"Feed:", u8g2_font_4x6_tr, 20, 43, &grblStatus.overRides[0], nullptr,0},
    {"Spindle:", u8g2_font_4x6_tr, 70, 43, &grblStatus.overRides[1], nullptr,0},
    {"Rapid:",u8g2_font_4x6_tr, 110, 43, &grblStatus.overRides[2], nullptr,0},
    {"State:",u8g2_font_profont12_tr  , 56, 30, nullptr, &grblStatus.status},
  };

const int runDynamicUiCount = sizeof(runDynamicUi) / sizeof(UiDynamicItem);


UiPage runPage = {
  "run",
  runMenu,
  runMenuCount,
  rootUi,
  rootUiCount,
  runDynamicUi,
  runDynamicUiCount,
  &rootPage
};

// *****************************************************
// currwent page pointer is what is going to be rendered
// *****************************************************
UiPage* currentPage = &rootPage;
// UiPage* currentPage = &setAxisPage;


