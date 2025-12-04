/*
 * functions for reading rotary encoders
 */

float shortestMove = 0.005;  // the shortest move that is being sent via jog
volatile int32_t encoderPos = 0;
String jogAxis = "x";
long lastPos = 0;           // Changed to non-static for access from main.ino

const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds

long timerEncoder = 0;

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
  DEBUG_PRINTLN("init encoder");
  lastPos = encoderPos;
}

// run every .1 sec
void readJogEncoder(String axis){
  static int32_t lastPos = 0;
  // Set active if encoder moved (based on encoderPos change)
  if (lastPos != encoderPos) {
    DEBUG_PRINTLN("encoder moved");
    timerEncoderRest = millis();
    if (grblStatus.uartMode == 0){
      DEBUG_PRINTLN("UART active");
      if (millis() - rtCmdTimer < 200) return;
      toggleEnable();
    }
    // send jog command
    encoderOut(axis);
    lastPos = encoderPos;
  }
}

/* 
 * reading the encoder position and preparing the jog command.
 * it is triggered every 100ms from the main loop via readJogEncoder()
 */
void encoderOut(String axis) {
  float difference = lastPos - encoderPos;
  float mm = (difference / (float(PULSES_PER_REVOLUTION) * 2)) * float(MM_PER_REVOLUTION);
  if (abs(mm) > shortestMove){
    String cmd = calculateCmd(mm, axis);
    sendToGrbl(cmd);
  }
  lastPos = encoderPos;
}

// builds the command with correct feedrate
String calculateCmd(float mm, String axis){
  // float feed = abs(mm * (60000 / float(interval)))*1.2;
  float feed = abs(mm * (60000 / float(interval)));
  String cmd = "$J=G91 ";
  cmd += axis;
  cmd += mm;
  cmd += " F";
  cmd += feed;
  cmd += "\n";
  return cmd;
}
