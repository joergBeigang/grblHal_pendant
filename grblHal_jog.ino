#include <config.h>
#include <encoder.h>

// #include <encoder.ino>
unsigned long previousMillis = 0;  // will store last time LED was updated
float shortestMove = 0.005;  // the shortest move that is being sent via jog
const long interval = 100;  // interval for sending jog commands in milliseconds



void setup() {
  // Initialize rotary encoder
  Serial.begin(115200);  // Match Serial Monitor to 115200
  pinMode(ENCODER_A, INPUT_PULLUP);  // Prevent floating pins
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
}


void loop() {
   unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {

    encoderOut();
    previousMillis = currentMillis;

    }
}
