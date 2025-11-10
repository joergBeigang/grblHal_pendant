#pragma once

struct MenuItem {
  const char* label;
  int x;
  int y;
  void (*action)();
  struct MenuItem* submenu;
  bool selected = false;
};

struct UiItem {
  const char* label;
  const uint8_t* font;
  int x;
  int y;
};

struct UiDynamicItem {
  const char* label;
  const uint8_t* font;
  int x;
  int y;
  float* value = nullptr;
  String* svalue = nullptr;
};

struct UiPage {
  const char* name;

  MenuItem* menuItems;
  int menuCount;

  UiItem* uiItems;
  int uiCount;

  UiDynamicItem* dynItems;
  int dynCount;
};

// Declare it as extern so other files can access it
extern MenuItem rootMenu[];
extern const int rootMenuCount;

extern UiItem rootUi[];
extern const int rootUiCount;

extern UiDynamicItem rootDynamicUi[];
extern const int rootUiCount;


extern UiPage rootPage;
