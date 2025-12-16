/*
This file is part of grblHal_pendant.
grblMachinist is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

grblMachinist is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
grblMachinist.
If not, see <https://www.gnu.org/licenses/>.
*/
// gui_build
// functions to render the GUI
// 128 x 64 display

void drawScreen(int index) {
  u8g2.clearBuffer();
  drawCursor(index);
  if (selectedIndex != -1) drawSelectionBox(selectedIndex);
  drawPage(*currentPage);
  u8g2.sendBuffer();
  }

// draw all the text on the screen
void drawPage(const UiPage& page) {
  u8g2.setDrawColor(1);
  // --- static UI ---
  for (int i = 0; i < page.uiCount; i++) {
    UiItem &item = page.uiItems[i];
    u8g2.setFont(item.font);
    u8g2.drawStr(item.x, item.y, item.label);
  }

  // --- dynamic UI ---
  char buf[24];
  for (int i = 0; i < page.dynCount; i++) {
    UiDynamicItem &item = page.dynItems[i];
    u8g2.setFont(item.font);
    //

    if (item.value) {
        char buf[20];
        char fmt[8];
        snprintf(fmt, sizeof(fmt), "%%.%df", item.digits);
        snprintf(buf, sizeof(buf), fmt, *(float*)item.value);
        u8g2.drawStr(item.x, item.y, buf);
    }
    if (item.svalue != nullptr) {
      u8g2.drawStr(item.x, item.y, item.svalue->c_str());
    }
  }

  // --- menu items ---
  for (int i = 0; i < page.menuCount; i++) {
    MenuItem &item = page.menuItems[i];
    u8g2.setFont(u8g2_font_5x8_tr);
    drawColor(i);
    if (item.value){
      int offset = center_dro(*item.value, 5);
      char buf[20];
      char fmt[8];
      int xpos = item.x + offset;
      snprintf(fmt, sizeof(fmt), "%%.%df", item.digits);
      snprintf(buf, sizeof(buf), fmt, *(float*)item.value);
      // u8g2.drawStr(item.x, item.y, buf);
      u8g2.drawStr(xpos, item.y, buf);
    }else{
        u8g2.drawStr(item.x, item.y, item.label);
    }
  }
}

// set the drawing color based on the selected item
void drawColor(int index){
  int color = 1;
  if (index == selectedIndex) color = 0;
  u8g2.setDrawColor(color);
}

// dreaw the curser rectangle
void drawSelectionBox(int index){
  MenuItem &item = currentPage->menuItems[index];
  int xPos = item.x;
  if (item.value) {
    int offset = center_dro(*item.value, 5);
    xPos += offset;
  }
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  u8g2.drawBox(xPos -2,  item.y-10,  width,  13);
}

// draw the black box over selected (active) menu items
void drawCursor(int index){
  MenuItem &item = currentPage->menuItems[index];
  int xPos = item.x;
  if (item.value) {
    int offset = center_dro(*item.value, 5);
    xPos += offset;
  }
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  // MenuItem &item = *currentPage.menuItems[i];
  // MenuItem &item = rootMenu[index];
  u8g2.drawFrame(xPos-2,  item.y-10,  width,  13);
}

// cente the dro around the period. 
// it's x position is defined by the position of the first character
// as longer the strig, as less it needs to be shifted to the left
// longest string to expect is -999.999, shortest 9.999 should return 
// for the longest 0, for the sortest 3, multiplied with the width of the 
// character
int center_dro(float number, int characterWidth) {
  int offset = 3;
  if (number < 0) offset --;
  if (abs(number) >= 10) offset --;
  if (abs(number) >= 100) offset --;

  return int(offset * characterWidth);
}

