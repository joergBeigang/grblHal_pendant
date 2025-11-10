// GUI drawing functions
// 128 x 64 display

void drawMainScreen(int index) {
  // u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.clearBuffer();
//  u8g2.setFont(u8g2_font_6x13_tr);
  // u8g2.setFont(u8g2_font_5x8_tr);
  
  // u8g2.setFont(u8g2_font_profont12_tr);
  // u8g2.drawStr(0, 10, "   X      Y     Z   ");
  // u8g2.setFont(u8g2_font_profont10_tr);
  // u8g2.drawStr(1, 20, "123.225 123.123 123.123");
  // u8g2.setFont(u8g2_font_4x6_tr);
  // u8g2.drawStr(0, 33, "feed 100 spindle 100 rapid 100");
  // u8g2.drawStr(50, 43, "Idle");

  // drawCursorMainMenu(index);
  // drawUiElements();
  // drawMainMenu();
  drawPage(rootPage);
  u8g2.sendBuffer();
  }

void drawNonInteractive(){
  u8g2.setFont(u8g2_font_profont12_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(10, 30, "Rotary Menu");
  u8g2.sendBuffer();
}

void drawMainMenu(){
  for (int i = 0; i<rootMenuCount; i++){
    MenuItem &item = rootMenu[i];
    u8g2.setFont(u8g2_font_5x8_tr);
    if (item.selected == false){
      u8g2.setDrawColor(1);
      u8g2.drawStr(item.x, item.y, item.label);
    } else {
      u8g2.setDrawColor(0);
      u8g2.drawBox(item.x-1, item.y-9, strlen(item.label) * 5 + 2, 10);
      u8g2.setDrawColor(1);
      u8g2.drawStr(item.x, item.y, item.label);
      u8g2.setDrawColor(1);
    }
  }
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
    if (item.value != nullptr) {
      // check if pointer is pointing somewhere valid

      // snprintf(buf, sizeof(buf), "%.3f %s",
      //          *(item.value)
      //          );
      snprintf(buf, sizeof(buf), "%.3f", *(item.value));
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
    u8g2.drawStr(item.x, item.y, item.label);
  }

}
// void drawPage(const UiPage& page) {
//   u8g2.clearBuffer();
//
//   // static UI
//   for (int i = 0; i < page.uiCount; i++) {
//     UiItem &item = page.uiItems[i];
//     u8g2.setFont(item.font);
//     u8g2.drawStr(item.x, item.y, item.label);
//   }
//
//   // dynamic UI
//     char buf[24];
//
//     for (int i = 0; i < page.dynCount; i++) {
//       UiDynamicItem& item = page.UiDynamicItem[i];
//       u8g2.setFont(item.font);
//
//       if (item.fvalue != nullptr) {
//         snprintf(buf, sizeof(buf), "%.3f %s", *(item.fvalue),
//                 item.unit ? item.unit : "");
//         u8g2.drawStr(item.x, item.y, buf);
//       } 
//       else if (item.svalue != nullptr) {
//         u8g2.drawStr(item.x, item.y, item.svalue->c_str());
//       }
//     }
//   // menu items
//   for (int i = 0; i < page.menuCount; i++) {
//     MenuItem &item = page.menuItems[i];
//     u8g2.setFont(u8g2_font_5x8_tr);
//     u8g2.drawStr(item.x, item.y, item.label);
//   }
//
//   u8g2.sendBuffer();
// }
void drawUiElements(){
  for (int i = 0; i<rootUiCount; i++){
    UiItem &item = rootUi[i];
    u8g2.setFont(item.font);
    u8g2.drawStr(item.x, item.y, item.label);
  }
}
  // draw the menu bar
// void drawMainMenu(){
//   u8g2.setFont(u8g2_font_5x8_tr);
//   u8g2.drawStr(3, 60, "Menu");
//   u8g2.drawStr(30, 60, "OFF");
//   u8g2.drawStr(55, 60, "X");
//   u8g2.drawStr(70, 60, "Y");
//   u8g2.drawStr(85, 60, "Z");
//   u8g2.drawStr(100, 60, "Joy");
//   }
//

void drawCursorMainMenu(int index){
  MenuItem &item = rootMenu[index];
  short width = strlen(item.label) * 5 + 4;  // 4px padding
  u8g2.drawFrame(item.x-2,  item.y-10,  width,  13);
}


