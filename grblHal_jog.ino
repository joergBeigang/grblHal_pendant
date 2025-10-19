#include <WiFi.h>
#include <config.h>
#include <WebSocketsClient.h>
#include <encoder.h>
#include <web_socket.h>


// #include <encoder.ino>
unsigned long previousMillis = 0;  // will store last time encoder was read
WebSocketsClient webSocket;




void setup() {
  // Initialize rotary encoder
  Serial.begin(115200);  // Match Serial Monitor to 115200
  // rotrary encoder for jog
  pinMode(ENCODER_A, INPUT_PULLUP);  // Prevent floating pins
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  // wifi setup
  // WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi...");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  Serial.println(" connected!");
  // websocket client setup
  // webSocket.begin("192.168.4.123", 81, "/"); // IP, port, URL
  // webSocket.onEvent(webSocketEvent);


}


void loop() {
  webSocket.loop();
  // read rotary encoder every .1 second
   unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {
    encoderOut();
    previousMillis = currentMillis;
    }
}
