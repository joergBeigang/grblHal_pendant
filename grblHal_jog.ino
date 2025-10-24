#include "config.h"
#include "encoder.h"

volatile int active = 0;           // Volatile for ISR access
const int buttonPin = 20;  
int buttonState = 0;    
int sent = 0;
// send real time command 0x8B to enable/disable uart command
void toggleEnable(){
  uint8_t byteToSend = 0x8B;
  Serial1.write(byteToSend); // sends exactly one byte
  Serial.write(byteToSend); // sends exactly one byte
  Serial.println("hex");
}



void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1); // UART0: TX=GP0, RX=GP1

  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  lastA = digitalRead(ENCODER_A);
  lastB = digitalRead(ENCODER_B);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
}

void loop() {
  buttonState = digitalRead(buttonPin);
   // check if the pushbutton is pressed. If it is, the buttonState is HIGH:

  if (buttonState == LOW) {
    // turn LED on:
    if (sent == 0){
      toggleEnable();
      sent = 1;
    }
  } else {
    // turn LED off:
    if (sent == 1){
    sent = 0;
    }
  }
  
  readJogEncoder();
  
  if (Serial1.available()) {
    char c = Serial1.read();
    Serial.print(c);  // forward UART data to USB Serial
  }
}
