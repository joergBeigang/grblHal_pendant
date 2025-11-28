#include <U8g2lib.h>
#include <Wire.h>
#include <math.h>
#include <Button.h>
#include "AiEsp32RotaryEncoder.h"
#include "config.h"
#include "encoder.h"
#include "joystick.h"
#include "parser.h"
#include "gui_render.h"
#include "gui_build.h"
#include "gui_menu_actions.h"
#include "ec11_encoder.h"
#include "ec11_encoder.h"

// if true = in control via uart
volatile bool active = false;

// volatile UiPage* nextPage = nullptr;

int cursorPosition = 0;          // where the cursor currently is
// const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds

bool ok = true;

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

// ISR – reading the EC11 encoders
void IRAM_ATTR readEncoderISR() {
  rotaryMenu.readEncoder_ISR();
  rotaryFeed.readEncoder_ISR();
  rotarySpindle.readEncoder_ISR();
}


// give grblStatus some initial values not avoid pointer errors
void initGrblStatus(){
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
  initGrblStatus();
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
  // pullups
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(MENU_A, INPUT_PULLUP);
  pinMode(MENU_B, INPUT_PULLUP);
  pinMode(FEED_A, INPUT_PULLUP);
  pinMode(FEED_B, INPUT_PULLUP);
  pinMode(SPINDLE_A, INPUT_PULLUP);
  pinMode(SPINDLE_B, INPUT_PULLUP);
  initRotaryEncoders();

  // variables for reading the high resolution encoder
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
    Serial2.write(byteToSend);
    Serial.write(byteToSend);
    Serial.println("ENABLE/DISABLE");
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
     // Serial.println("encoder x");
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


// read UART from grblHAL
void readUart(){
  static String grblInfo;
  while (Serial2.available()) {
    char c = Serial2.read();
    grblInfo += c;

    if (c == '\n') {
        // Serial.print("LINE: ");
        // Serial.println(grblInfo);
        // check for ok
        if (grblInfo.indexOf("ok") != -1) {
          ok = true;
        }

        bool chk = parseGrblStatusReport(grblInfo);
        if (chk == true){
          // set operation mode based on grbl status
          // setMode();
          // redraw the screen with the updated info
          // drawScreen(cursorPosition);  // redraw cursor
        } 
        grblInfo = "";    // reset after full line is received
    }
  }
}

void loop() {
  rotaryMenuLoop();
  overRideSwitches();
  readUart();
  processQueue();

  static unsigned long timerJog = 0;  // will store last time encoder was read
  unsigned long currentMillis = millis();
  if (currentMillis - timerJog >= 100) {
    // set the right mode based on grblhal's status
    modeLogic();
    // update feed and spindle overrides 
    rotaryFeedLoop();
    rotarySpindleLoop();
    // redreaw the screen
    drawScreen(cursorPosition);
    // reset the timer
    timerJog = currentMillis;
  }

}
