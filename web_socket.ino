#include <WebSocketsClient.h>


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println("Connected to server");
    webSocket.sendTXT("?");
    webSocket.sendTXT("$$");
  }
  if (type == WStype_TEXT) {
    Serial.printf("Server says: %s\n", payload);
  }
}

