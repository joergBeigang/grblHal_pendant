#include <math.h>
#include "config.h"
#include "encoder.h"
#include "joystick.h"

volatile int active = 0;           // Volatile for ISR access
const int buttonPin = 20;  
  
int joyXCenter = 1284;
int joyYCenter = 1744;
float joyXMax = 0.6; // max value for X axis
float joyYMin = 0.85; // min value for Y axis
  
  
void setup() {
  // Serial
  // Serial usb for debugging
  Serial.begin(115200);
  // Serial uart for grblHAL communication
  Serial2.begin(115200, SERIAL_8N1, GRBL_RX, GRBL_TX);

  // read joystick
  analogReadResolution(12);

  // Rotary encoder for Jogging
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  lastA = digitalRead(ENCODER_A);
  lastB = digitalRead(ENCODER_B);
  // interrupts for encoder reading
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
}

  // send real time command 0x8B to enable/disable uart command
  void toggleEnable(){
    uint8_t byteToSend = 0x8B;
    Serial2.write(byteToSend); // sends exactly one byte
    Serial.write(byteToSend); // sends exactly one byte
    Serial.println("hex");
  }


  

void loop() {
  
  // readJogEncoder();
  // String test = "<Run,MPos:1.111,2.222,3.333,WPos:4.444,5.555,6.666>";
  // parseGrblStatus(test);
  // Serial.println(grbl.position_raw[0]);
  // String state = parseGrblState(test);
  // Serial.println(state);
  readJoystick();

  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);  // forward UART data to USB Serial
  }
  delay(100);
}
