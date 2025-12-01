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

void actionUnlock();
void actionReset();
void actionHoming();
void actionHomingMenu();
void actionConfimrHomingMenu();
void actionSetAxisZ();
void actionSetAxisY();
void actionSetAxisX();
void actionCancel();
extern UiPage* currentPage;
// extern volatile UiPage* nextPage;  // pointer to page we want to switch to safely
