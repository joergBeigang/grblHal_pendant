
/*
 * all functions for the ec11 encoder 
 * for feed and spindle override 
 * and menu
 */

int lastValue = 0;
bool setValueMode = false;
int rotatryMenuLastPos = 0;
float valueEditIncrement = .1;
float valueEditMin = -9999;
float valueEditMax = 9999;

// instace stuct with rt commands for overrides
Overrides feedOv = {0, 0x93, 0x91, 0x94, 0x92};
Overrides spindleOv = {0, 0x9C, 0x9A, 0x9D, 0x9B};


// Setup EC11 Rotary Encoders
void initRotaryEncoders() {
  rotaryMenu.begin();
  rotaryMenu.setup(readEncoderISR);
  rotaryMenu.disableAcceleration();

  rotaryFeed.begin();
  rotaryFeed.setup(readEncoderISR);
  rotaryFeed.disableAcceleration();

  rotarySpindle.begin();
  rotarySpindle.setup(readEncoderISR);
  rotarySpindle.disableAcceleration();
  // buttons 
  buttonMenu.begin();
  buttonFeed.begin();
  buttonSpindle.begin();
}


// *********
// overrides
// *********

void sendOverrideCommands(Overrides &ov){
  if (ov.difference > 10) {
    Serial2.write(ov.cmdTen);
    ov.difference -= 10;
  } else if (ov.difference > 0) {
    Serial2.write(ov.cmdOne);
    ov.difference -= 1;
  }
  if (ov.difference < -10) {
    Serial2.write(ov.cmdTenMinus);
    ov.difference += 10;
  } else if (ov.difference < 0) {
    Serial2.write(ov.cmdOneMinus);
    ov.difference += 1;
  }
}

// setting feed or spindle override via button release back to 100
void overRideSwitches(){
	if (buttonFeed.released()) {
    DEBUG_PRINTLN("feed 100");
    uint8_t byteToSend = 0x90;
    Serial2.write(byteToSend); 
  }
	if (buttonSpindle.released()) {
    DEBUG_PRINTLN("spindle 100");
    uint8_t byteToSend = 0x99;
    Serial2.write(byteToSend); 
  }
}


void rotaryFeedLoop(){
  static int rotatryFeedLastPos = 0;
  if (rotaryFeed.encoderChanged()) {
    int movement = rotaryFeed.readEncoder();  // cumulative steps since last reset
    int dif = movement - rotatryFeedLastPos;
    rotatryFeedLastPos = movement;
    feedOv.difference += dif;
  }
  sendOverrideCommands(feedOv);
}

void rotarySpindleLoop(){
  static int rotatrySpindleLastPos = 0;
  if (rotarySpindle.encoderChanged()) {
    int movement = rotarySpindle.readEncoder();  // cumulative steps since last reset
    int dif = movement - rotatrySpindleLastPos;
    rotatrySpindleLastPos= movement;
    spindleOv.difference += dif;
  }
  sendOverrideCommands(spindleOv);
}


// **************
// menu functions
// **************

void setValue(){
  int inverter = 1;
  if (MENU_INVERT == 1){
    inverter = -1;
  }
    int movement = rotaryMenu.readEncoder();  // cumulative steps since last reset
                                              // S
    valueEdit = float((movement * valueEditIncrement)*inverter);
    if (valueEdit < valueEditMin) valueEdit = valueEditMin;
    if (valueEdit > valueEditMax) valueEdit = valueEditMax;
}

void moveCursor(){
  int inverter = 1;
  if (MENU_INVERT == 1){
    inverter = -1;
  }
    int movement = rotaryMenu.readEncoder();  // cumulative steps since last reset
    int dif =(movement-rotatryMenuLastPos) * inverter;
    cursorPosition += dif;
    if (cursorPosition < 0) cursorPosition = 0;
    if (cursorPosition >= currentPage -> menuCount) cursorPosition = currentPage -> menuCount - 1;
      rotatryMenuLastPos = movement;
}

void rotaryMenuLoop(){
  if (rotaryMenu.encoderChanged()) {
    if (setValueMode == false) moveCursor();
    if (setValueMode == true) setValue();
    drawScreen(cursorPosition);
    }
	
	if (buttonMenu.released()) {
    // drawScreen(cursorPosition);
    // MenuItem &item = rootMenu[cursorPosition];
    MenuItem &item = currentPage->menuItems[cursorPosition];

    if (item.action != nullptr){
      item.action();
    }
  }
}

