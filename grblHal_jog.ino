#include <U8g2lib.h>
#include <Wire.h>
#include <math.h>
#include "AiEsp32RotaryEncoder.h"
#include "config.h"
#include "encoder.h"
#include "joystick.h"
#include "parser.h"
#include "gui_render.h"
#include "gui_build.h"
#include "gui_menu_actions.h"
#include <Button.h>

volatile bool active = false;       // if true = in control via uart
volatile UiPage* nextPage = nullptr;
int cursorPosition = 0;          // where the cursor currently is
int selectedIndex = -1;          // 1 = off is selected on startup
// const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds
//
// mode = operation mode
// 0 - off
// 1 - joystick mode
// 2 - X axis encoder mode
// 3 - Y axis encoder mode
// 5 - machine is in run state
int mode = 0;                   

// the value that is edited in the menu via menu encoder
float valueEdit = 0.0;  

// pointer to valueEdit to use in struct (expects pointer)
float  *pValueEdit = &valueEdit;

int lastValue = 0;
bool setValueMode = false;

// instace of struct that collects all info send by grbl
GrblStatus grblStatus;

// display
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// E11 rotary encoders
// rotary encoder menu
AiEsp32RotaryEncoder rotaryMenu(MENU_A, MENU_B, -1, -1, 4);
// rotary encoder feed override
AiEsp32RotaryEncoder rotarySpindle(SPINDLE_A, SPINDLE_B, -1, -1, 4);
// rotary encoder spindle override
AiEsp32RotaryEncoder rotaryFeed(FEED_A, FEED_B, -1, -1, 4);

// buttons of rotary encoders, button libraray for easier release detection
Button buttonMenu(MENU_SW);
Button buttonFeed(FEED_SW);
Button buttonSpindle(SPINDLE_SW);

// ISR – reading the high resolution rotary encoder for Jogging
// no debounce here, fast reading of 1200 pulses per rev
void IRAM_ATTR readEncoderISR() {
  rotaryMenu.readEncoder_ISR();
  rotaryFeed.readEncoder_ISR();
  rotarySpindle.readEncoder_ISR();
}

// Setup EC11 Rotary Encoders
void initRotaryEncoders() {
  rotaryMenu.begin();
  rotaryMenu.setup(readEncoderISR);
  rotaryMenu.disableAcceleration();

  rotaryFeed.begin();
  rotaryFeed.setup(readEncoderISR);

  rotarySpindle.begin();
  rotarySpindle.setup(readEncoderISR);
  // buttons 
  buttonMenu.begin();
  buttonFeed.begin();
  buttonSpindle.begin();
}

void grblStatusInit(){

  grblStatus.status = "NONE";
  grblStatus.position[0] = 990.0;
  grblStatus.position[1] = 120.0;
  grblStatus.position[2] = 183.0;
  grblStatus.overRides[0] = 100.0;
  grblStatus.overRides[1] = 100.0;
  grblStatus.overRides[2] = 100.0;
}
void setup() {
  // initial values of grblStatus
  grblStatusInit();

  // Serial
  // Serial usb for debugging
  Serial.begin(115200);
  // Serial uart for grblHAL communication
  Serial2.begin(115200, SERIAL_8N1, GRBL_RX, GRBL_TX);
  // wire for display
  Wire.begin(21, 22);
  u8g2.begin();
  // analog read for joystick
  analogReadResolution(12);
  // Rotary encoder for Jogging
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(MENU_A, INPUT_PULLUP);
  pinMode(MENU_B, INPUT_PULLUP);
  pinMode(FEED_A, INPUT_PULLUP);
  pinMode(FEED_B, INPUT_PULLUP);
  pinMode(SPINDLE_A, INPUT_PULLUP);
  pinMode(SPINDLE_B, INPUT_PULLUP);

  initRotaryEncoders();
  lastA = digitalRead(ENCODER_A);
  lastB = digitalRead(ENCODER_B);

  // interrupts for encoder reading
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
  drawScreen(0);
}

  // send real time command 0x8B to enable/disable uart command
void toggleEnable(){
    uint8_t byteToSend = 0x8B;
    Serial2.write(byteToSend); // sends exactly one byte
    Serial.write(byteToSend); // sends exactly one byte
    Serial.println("hex");
    // drawMainScreen(0);
  }


int rotatryMenuLastPos = 0;


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


void setValue(){
  Serial.println("setting value");
  int inverter = 1;
  if (MENU_INVERT == 1){
    inverter = -1;
  }
    int movement = rotaryMenu.readEncoder();  // cumulative steps since last reset
                                              // S
    valueEdit = float((movement * 0.1)*inverter);
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
    if(nextPage != nullptr){
      currentPage = const_cast<UiPage*>(nextPage);  // remove volatile
      // currentPage = nextPage; // safely change page
      // selectedIndex = 0;
      nextPage = nullptr;
      drawScreen(0);          // safe draw
    }
  }
}


void rotaryFeedLoop(){
  static int rotatryFeedLastPos = 0;
  if (rotaryFeed.encoderChanged()) {
    Serial.println("feed");
      int inverter = 1;
      int movement = rotaryFeed.readEncoder();  // cumulative steps since last reset
      // int dif =(movement-rotatryFeedLastPos) * inverter;
      grblStatus.overRides[0] = movement;
      rotatryMenuLastPos = grblStatus.overRides[0];
      // wrap around

      drawScreen(cursorPosition);  // redraw cursor
  }
}


void rotarySpindleLoop(){
  static int rotatrySpindleLastPos = 0;
  if (rotarySpindle.encoderChanged()) {
    Serial.println("spindle");
      int inverter = 1;
      int movement = rotarySpindle.readEncoder();  // cumulative steps since last reset
      grblStatus.overRides[1] = movement ;
      // wrap around

      drawScreen(cursorPosition);  // redraw cursor
  }
}


// triggered every .1 seconds
void modeLogic(){
 switch (mode){
   case 0:
     break;
   case 1:
     readJoystick();
     break;
   case 2:
     readJogEncoder(String("X"));
     break;
   case 3:
     readJogEncoder(String("Y"));
     break;
   case 5:
     currentPage = &runPage;
     break;
 }
}


// setting the mode based on the machine status
// run everytime the parser found a grbl status report
void setMode(){
  int previousMode = mode;
  if (grblStatus.status == String("run")){
    // machine is runnig some gcode. we can change the screen here to have 
    // a pause button
    mode = 5;
    return;
  }
  if (grblStatus.status == String("idle") && previousMode == 5){
    // swtich the page back to root page
    currentPage = &rootPage;
    mode = 0; //jogging off
    return;
  }
  if (grblStatus.status == String("jog")){
    return;
  }

  // some other state like alarm, door open etc. 
  // jogging is switched off
  mode = 0;
}

// setting feed or spindle override via button release back to 100
void overRideSwitches(){
	if (buttonFeed.released()) {
    Serial.println("feed 100");
  }
	if (buttonSpindle.released()) {
    Serial.println("spindle 100");
  }
}

void loop() {
  rotaryMenuLoop();
  // String test = "<Hold:0|MPos:-273.141,-109.772,-4.059|Bf:100,1019|FS:0,0|Pn:HSO|WCO:-319.950,-194.084,-67.363>";
  // parseGrblStatusReport(test);
  // readJoystick();
  // for (int i = 0; i < 6; i++){
    // drawMainScreen(i);
    // delay(200);
  // }

  // read UART from grblHAL
  static String grblInfo;
  while (Serial2.available()) {
    char c = Serial2.read();
    grblInfo += c;

    if (c == '\n') {
        Serial.print("LINE: ");
        Serial.println(grblInfo);
        bool chk = parseGrblStatusReport(grblInfo);
        if (chk == true) setMode();
        grblInfo = "";    // reset after full line is received
    }
  }
  overRideSwitches();
  static unsigned long timerJog = 0;  // will store last time encoder was read
  unsigned long currentMillis = millis();
  if (currentMillis - timerJog >= 100) {
    modeLogic();
    rotaryFeedLoop();
    rotarySpindleLoop();

    timerJog = currentMillis;
  }

}
