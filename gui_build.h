#pragma once

struct MenuItem {
  const char* label;
  int x;
  int y;
  void (*action)();
  struct MenuItem* submenu;
  bool selected = false;
  float* value = nullptr;
  int digits = 3;
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
  int digits = 3;
};

struct UiPage {
  const char* name;

  MenuItem* menuItems;
  int menuCount;

  UiItem* uiItems;
  int uiCount;

  UiDynamicItem* dynItems;
  int dynCount;

  struct UiPage* parent;
};

// Declare it as extern so other files can access it
// root menu
extern MenuItem rootMenu[];
extern const int rootMenuCount;

extern UiItem rootUi[];
extern const int rootUiCount;

extern UiDynamicItem rootDynamicUi[];
extern const int rootUiCount;

extern UiPage rootPage;

extern UiPage rootPage;


// set axis menu
extern MenuItem setAxisMenu[];
extern const int setAxisMenuCount;

extern UiItem setAxisXUi[];
extern const int setAxisXUiCount;


extern UiDynamicItem setAxisDynamicUi[];
extern const int setAxisDynamniUiCount;

extern MenuItem menuMenu[];
extern const int menuMenuCount;

extern UiPage setAxisXPage;
extern UiPage setAxisyPage;
extern UiPage setAxiszPage;
extern UiPage menuPage;
extern UiPage confirmHomingPage;
extern UiPage runPage;

extern UiPage* currentPage;




