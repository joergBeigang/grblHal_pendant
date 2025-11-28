

#include "gui_build.h"


// **********************
// common actions
// **********************

// set an axis to a specified value
void setAxisToValue(String axis) {
  // get the active coordinate system
  int co = grblStatus.coordinateSystem;
  co -= 3;

  // cmd = f"G10 L20 P{num} {direction} {value}"
  String cmd = "G10 L20 P";
  cmd += String(co);
  cmd += axis;
  cmd += String(valueEdit);
  sendSingleCommand(cmd);
}

void actionSetValue(){
  if (setValueMode == false){
    setValueMode = true;
    selectedIndex = cursorPosition;
  } else {
    setValueMode = false;
    selectedIndex = -1;
  }
}

void actionCancel(){
      Serial.println("cancel");
    Serial.printf("currentPage=%p, parent=%p\n", currentPage, currentPage->parent);
  Serial.println("cancel");
  valueEdit = 0;
  // currentPage = &rootPage;
  currentPage = currentPage -> parent;
  selectedIndex = -1;
  // nextPage = &rootPage;
}




// **********************
// actions for main menu items
// **********************
void actionOff(){
  // for (int i = 0; i<rootMenuCount; i++){
  //   MenuItem &item = rootMenu[i];
  //   // item.selected = (i == cursorPosition);
  // }
  mode = 0;
  selectedIndex = cursorPosition;
  // drawScreen(cursorPosition);
}

void actionMenu() {
  currentPage = &menuPage;
  selectedIndex = -1;
  cursorPosition = 0;
  mode = 0;
}
void actionX() {
  selectedIndex = cursorPosition;
  // drawScreen(cursorPosition);
  initEncoder();
  mode = 2;
}
void actionY() {
  selectedIndex = cursorPosition;
  // drawScreen(cursorPosition);
  initEncoder();
  mode = 3;
}
void actionZ() {
  
  selectedIndex = cursorPosition;
  // drawScreen(cursorPosition);
}
void actionJoy() {
  selectedIndex = cursorPosition;
  // drawScreen(cursorPosition);
  initJoystick();
  mode = 1;
}
void actionXPos() {
  mode = 0;
  currentPage = &setAxisXPage;
  // request the active coordinate system
  sendSingleCommand("$G");
  cursorPosition = 0;
  selectedIndex = -1;
}
void actionYPos() {
  mode = 0;
  currentPage = &setAxisYPage;
  // request the active coordinate system
  sendSingleCommand("$G");
  cursorPosition = 0;
  selectedIndex = -1;
  Serial.println("action");
}
void actionZPos() {
  mode = 0;
  currentPage = &setAxisZPage;
  // request the active coordinate system
  sendSingleCommand("$G");
  cursorPosition = 0;
  selectedIndex = -1;
  Serial.println("action");
}


// **********************
// actions for set x axis
// **********************
void actionSetAxisX(){
  setAxisToValue("X");
  valueEdit = 0;
  currentPage = &rootPage;
  selectedIndex = -1;
}

// **********************
// actions for set y axis
// **********************
void actionSetAxisY(){
  setAxisToValue("Y");
  valueEdit = 0;
  currentPage = &rootPage;
  selectedIndex = -1;
}

// **********************
// actions for set x axis
// **********************
void actionSetAxisZ(){
  setAxisToValue("Z");
  valueEdit = 0;
  currentPage = &rootPage;
  selectedIndex = -1;
}


void actionHomingMenu() {
  currentPage = &confirmHomingPage;
  cursorPosition = 0;
  // drawScreen(cursorPosition);
  selectedIndex = -1;
}

void actionHoming() {
  sendSingleCommand("$H");
}

void actionReset() {
  currentPage = &rootPage;
  cursorPosition = 0;
  uint8_t reset = 0x18; 
  Serial2.write(reset);
}

void actionUnlock() {
  currentPage = &rootPage;
  cursorPosition = 0;
  sendSingleCommand("$X");

}
