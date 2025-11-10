// gui_build
// functions to render the GUI
// 128 x 64 display

void drawScreen(int index) {
  u8g2.clearBuffer();

  drawCursor(index);
  drawPage(*currentPage);
  u8g2.sendBuffer();
  }

void drawPage(const UiPage& page) {

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
    if (item.value){
        char buf[20];
        char fmt[8];
        snprintf(fmt, sizeof(fmt), "%%.%df", item.digits);
        snprintf(buf, sizeof(buf), fmt, *(float*)item.value);
        u8g2.drawStr(item.x, item.y, buf);
    }else{
        u8g2.drawStr(item.x, item.y, item.label);
    }
    // u8g2.drawStr(item.x, item.y, item.label);

  }

}
void drawUiElements(){
  for (int i = 0; i<rootUiCount; i++){
    UiItem &item = rootUi[i];
    u8g2.setFont(item.font);
    u8g2.drawStr(item.x, item.y, item.label);
  }
}

void drawCursor(int index){

  MenuItem &item = currentPage->menuItems[index];
  // MenuItem &item = *currentPage.menuItems[i];
  // MenuItem &item = rootMenu[index];
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  u8g2.drawFrame(item.x-2,  item.y-10,  width,  13);
}


