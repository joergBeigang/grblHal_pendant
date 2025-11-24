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
    // Serial.println(item.value)
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
        char buf[20];
        char fmt[8];
        snprintf(fmt, sizeof(fmt), "%%.%df", item.digits);
        snprintf(buf, sizeof(buf), fmt, *(float*)item.value);
        u8g2.drawStr(item.x, item.y, buf);
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


// void drawUiElements(){
//   for (int i = 0; i<rootUiCount; i++){
//     UiItem &item = rootUi[i];
//     u8g2.setFont(item.font);
//     u8g2.drawStr(item.x, item.y, item.label);
//   }
// }

// dreaw the curser rectangle
void drawSelectionBox(int index){
  MenuItem &item = currentPage->menuItems[index];
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  u8g2.drawBox(item.x-2,  item.y-10,  width,  13);
}

// draw the black box over selected (active) menu items
void drawCursor(int index){
  MenuItem &item = currentPage->menuItems[index];
  // MenuItem &item = *currentPage.menuItems[i];
  // MenuItem &item = rootMenu[index];
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  u8g2.drawFrame(item.x-2,  item.y-10,  width,  13);
}


