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

#include <U8g2lib.h>
#include <Wire.h>
#include <math.h>
#include <Button.h>
#include "AiEsp32RotaryEncoder.h"
#include "config.h"
#include "settings.h"
#include "encoder.h"
#include "joystick.h"
#include "parser.h"
#include "gui_render.h"
#include "gui_build.h"
#include "gui_menu_actions.h"
#include <Preferences.h>
#include "ec11_encoder.h"
#include "stream.h"

Preferences prefs;
// instace the settings struct
Settings settings;

// selected menu item
int selectedIndex = -1;          // 1 = off is selected on startup

// to keep UART active while
volatile bool keepActive = false; 

// timer for real time command, used for swtiching on in 
// joystick and encoder reading to avoid too fast switching
// and grblHal freezinguG
long rtCmdTimer = 0;

// timer for swtiching off UART Mode. 
// needs to be reset to keep UART active
// used for jogging with encoder and joyustick
long timerEncoderRest  = 0;

// position of the cursor in the menu
int cursorPosition = 0; 

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
  grblStatus.position[0] = -331.0;
  grblStatus.position[1] = -912.0;
  grblStatus.position[2] = -183.0;
  grblStatus.overRides[0] = 100.0;
  grblStatus.overRides[1] = 100.0;
  grblStatus.overRides[2] = 100.0;
  grblStatus.coordinateSystem = 54;
}


void setup() {
  // initial values of grblStatus
  initGrblStatus();
  // peferences
  prefs.begin("settings");
  readSettings();
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

  // check if there are valid joystick settings set, if not start 
  // the calibration process
  checkJoystickSettings();
}

// send real time command 0x8B to enable/disable uart command
void toggleEnable(){
  uint8_t byteToSend = 0x8B;
  Serial2.write(byteToSend);
  rtCmdTimer = millis();
  DEBUG_PRINTLN("ENABLE/DISABLE");
  DEBUG_PRINTLN(grblStatus.uartMode);

  if (grblStatus.uartMode == true) {
    timerEncoderRest = millis();
  }
}

// disable UART mode after a defined period without any user 
// input via joustick or rotatary encoder
void disableTimer(){
  if (keepActive == true) {
    timerEncoderRest = millis();
  };
  long currentMillis = millis();

  if (currentMillis - timerEncoderRest >= CANCEL_UART_TIMER) {
  // switch off uart mode
      if (grblStatus.uartMode == 1){
        resetQueue();
        toggleEnable();
        DEBUG_PRINTLN("UAERT off");
      }
  }
}

// triggered every .1 seconds
void modeLogic(){
 switch (mode){
   case 0:
     if (currentPage == &rootPage) selectedIndex = 1;
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
    cursorPosition = 0;
     break;
 }
}


// setting the mode based on the machine status
// run everytime the parser found a grbl status report
void setMode(){
  static int previousMode = mode;
  int chk1 = grblStatus.status.indexOf("RUN"); 
  int chk2 = grblStatus.status.indexOf("HOLD"); 
  if (chk1 != -1 || chk2 != -1) {
    mode = 5;
    previousMode = 5;
    currentPage = &runPage;
    selectedIndex = 1;  // pause button is highlighted
    return;
  } else {
    if (previousMode == 5){
      currentPage = &rootPage;
      // machine just stopped running, go back to previous mode
      mode = 0;
      selectedIndex = 1; // off is highlighted
      previousMode = 0;
      return;
    }
  }

}


// read UART from grblHAL
void readUart(){
  static String grblInfo;
  while (Serial2.available()) {
    char c = Serial2.read();
    grblInfo += c;

    if (c == '\n') {
        setMode();
        bool chk = parseGrblOutput(grblInfo);
        grblInfo = "";    // reset after full line is received
    }
  }
}

void loop() {
  // timer for disabling uart mode automatically after 1 sec of not using it
  disableTimer();
  // read the rotary encoders for menu
  rotaryMenuLoop();
  // switches of all ec11 rotary encoders
  overRideSwitches();
  // read grblHals ouput
  readUart();
  // stream gcode to grblHal
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
