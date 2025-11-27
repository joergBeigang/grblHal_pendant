/*
 * functions for reading rotary encoders
 */

float shortestMove = 0.005;  // the shortest move that is being sent via jog
volatile int32_t encoderPos = 0;
String jogAxis = "x";
long lastPos = 0;           // Changed to non-static for access from main.ino
//volatile bool lastA = 0;
//volatile bool lastB = 0;

const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds

long timerEncoder = 0;
long timerEncoderRest  = 0;

// interrupt based encoder reading
void encoderISR() {   // just remove IRAM_ATTR
  bool a = digitalRead(ENCODER_A);
  bool b = digitalRead(ENCODER_B);

  if (a != lastA) {
    encoderPos += (a == b) ? 1 : -1;
  }
  lastA = a;
  lastB = b;
}

// initialize the encoder to avoid unwanted movment when activating
void initEncoder() {
  lastPos = encoderPos;
}

// run every .1 sec
void readJogEncoder(String axis){
  static int32_t lastPos = 0;
  Serial.println(encoderPos);
  // Set active if encoder moved (based on encoderPos change)
  if (lastPos != encoderPos) {
    if (active == 0){
      Serial.println("active");
      toggleEnable();
      active = 1;
    }
    // send jog command
    encoderOut(axis);
    lastPos = encoderPos;
  }
  else{
 // if (currentMillis - timerEncoderRest >= 1000) {
 // switch off uart mode
    if (active == 1){
      active = 0;
      toggleEnable();
      Serial.println("off");
    }
  }
}

/* 
 * reading the encoder position and preparing the jog command.
 * it is triggered every 100ms from the main loop via readJogEncoder()
 */
void encoderOut(String axis) {
  // uint8_t info = 0x80;
  // Serial1.write(info);  
  // if (encoderPos == lastPos) {
  //   return;  // No change in position
  // }
  float difference = lastPos - encoderPos;
  float mm = (difference / (float(PULSES_PER_REVOLUTION) * 2)) * float(MM_PER_REVOLUTION);
  if (abs(mm) > shortestMove){
    String cmd = calculateCmd(mm, axis);
    // Serial.println(difference);
    // Serial.println(cmd);
    // wsClient.sendTXT(cmd);
    // Serial1.write(cmd.c_str(), cmd.length());
    Serial2.print(cmd);
  }
  lastPos = encoderPos;
}

// builds the command with correct feedrate
String calculateCmd(float mm, String axis){
  float feed = abs(mm * (60000 / float(interval)));
  String cmd = "$J=G91 ";
  cmd += axis;
  cmd += mm;
  cmd += " F";
  cmd += feed;
  cmd += "\n";
  return cmd;
}
