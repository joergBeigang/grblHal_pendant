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
unsigned long timerJog = 0;  // will store last time encoder was read
unsigned long timerJogRest = 0;  // will store last time encoder was read
volatile UiPage* nextPage = nullptr;
bool isEncoderButtonDown = 0;
// float test = 10;
int cursorPosition = 0;          // where the cursor currently is
int selectedIndex = -1;          // 1 = off is selected on startup
// const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds
int mode = 0;                      // 0 = None, 1 = Encoder, 2 = Joystick
                                   //
float valueEdit = 0.0;  // value for editing in the menu
int lastValue = 0;
float  *test = &valueEdit;
bool setValueMode = false;

GrblStatus grblStatus;
// display

U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// rotary encoder menu
AiEsp32RotaryEncoder rotaryMenu(MENU_A, MENU_B, -1, -1, 4);
// rotary encoder feed override
AiEsp32RotaryEncoder rotaryFeed(FEED_A, FEED_B, FEED_SW, -1, 4);
// rotary encoder spindle override
AiEsp32RotaryEncoder rotarySpindle(SPINDLE_A, SPINDLE_B, SPINDLE_SW, -1, 4);

Button buttonMenu(MENU_SW);

// ISR – NO delay(), NO Serial, NO heavy code!
void IRAM_ATTR readEncoderISR() {
  rotaryMenu.readEncoder_ISR();
  rotaryFeed.readEncoder_ISR();
  rotarySpindle.readEncoder_ISR();
}

void setupRotaryEncoders() {
  // Setup Rotary Encoders
  rotaryMenu.begin();
  rotaryMenu.setup(readEncoderISR);
  rotaryMenu.disableAcceleration();
  buttonMenu.begin();

  rotaryFeed.begin();
  rotaryFeed.setBoundaries(20, 200, true);
  rotaryFeed.setup(readEncoderISR);

  rotarySpindle.begin();
  rotarySpindle.setBoundaries(20, 200, true);
  rotarySpindle.setup(readEncoderISR);


}







void setup() {
  grblStatus.status = "NONE";
  grblStatus.position[0] = 990.0;
  grblStatus.position[1] = 120.0;
  grblStatus.position[2] = 183.0;
  grblStatus.overRides[0] = 100.0;
  grblStatus.overRides[1] = 100.0;
  grblStatus.overRides[2] = 100.0;
  // Serial
  // Serial usb for debugging
  Serial.begin(115200);
  // Serial uart for grblHAL communication
  Serial2.begin(115200, SERIAL_8N1, GRBL_RX, GRBL_TX);
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

  setupRotaryEncoders();
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

// void jog(){
//   static int32_t lastPos = 0;
//   unsigned long currentMillis = millis();
//   if (mode == 0){
//     return;
//   }
//
//   if (currentMillis - timerJog >= interval) {
//     // Set active if encoder moved (based on encoderPos change)
//       if (lastPos != encoderPos) {
//         if (active == 0){
//         Serial.println("active");
//         toggleEnable();
//         active = 1;
//         }
//         // send jog command
//         if (mode == 1){
//             readEncoder();
//         }
//         encoderOut();
//         lastPos = encoderPos;
//         timerJogRest = currentMillis;
//     }
//     timerJog = currentMillis;
//     }
//     if (currentMillis - timerJogRest >= 1000) {
//         if (active == 1){
//             active = 0;
//             toggleEnable();
//             Serial.println("off");
//         }
//         timerJogRest = currentMillis;
//     }
//
// }
int rotatryMenuLastPos = 0;

int rotatryFeedLastPos = 0;

int rotatrySpindleLastPos = 0;

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
  if (rotarySpindle.encoderChanged()) {
    Serial.println("spindle");
      int inverter = 1;
      int movement = rotarySpindle.readEncoder();  // cumulative steps since last reset
      grblStatus.overRides[1] = movement ;
      // wrap around

      drawScreen(cursorPosition);  // redraw cursor
  }
}
void loop() {
    rotaryMenuLoop();
    rotaryFeedLoop();
    rotarySpindleLoop();
    // String test = "<Hold:0|MPos:-273.141,-109.772,-4.059|Bf:100,1019|FS:0,0|Pn:HSO|WCO:-319.950,-194.084,-67.363>";
    // parseGrblStatusReport(test);
    // readJoystick();
    // for (int i = 0; i < 6; i++){
      // drawMainScreen(i);
      // delay(200);
    // }


    // read UART from grblHAL
    if (Serial2.available()) {
      char c = Serial2.read();
      Serial.print(c);  // forward UART data to USB Serial
    }
}
