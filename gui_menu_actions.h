#include "gui_build.h"

#pragma once
void actionOff();
void actionMenu();
void actionX();
void actionY();
void actionZ();
void actionJoy();
void actionXPos();
void actionYPos();
void actionZPos();

void actionConfimrHomingMenu();

void actionSetAxisX();
void actionCancel();
extern UiPage* currentPage;
// extern volatile UiPage* nextPage;  // pointer to page we want to switch to safely
