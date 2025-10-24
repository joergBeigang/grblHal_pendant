// #include <WebSocketsClient.h>
//
//
// void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
//   if (type == WStype_CONNECTED) {
//     Serial.println("Connected to server");
//     webSocket.sendTXT("?");
//     webSocket.sendTXT("$$");
//   }
//   if (type == WStype_TEXT) {
//     Serial.printf("Server says: %s\n", payload);
//   }
// }
//
// // ---- WebSocket server callback ----
// void wsServerEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//   if (type == WStype_CONNECTED) {
//     Serial.printf("Client %u connected\n", num);
//   } else if (type == WStype_DISCONNECTED) {
//     Serial.printf("Client %u disconnected\n", num);
//   } else if (type == WStype_TEXT) {
//     String msg = String((char*)payload);
//     Serial.printf("Client says: %s\n", msg.c_str());
//     if (wsClient.isConnected()) {
//       wsClient.sendTXT(msg); // forward to GRBLHAL
//     }
//   }
// }
