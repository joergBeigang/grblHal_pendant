/*
 * functions for reading rotary encoders
 */


long encoderPos = 0;  // Use long for high PPR (1200)
String jogAxis = "x";
static long lastPos = 0;  // Print initial position
/* encoder reading interrupt
 * for the high resolution encoder for jogging the machine
 */
void IRAM_ATTR encoderISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = micros();
  if (interruptTime - lastInterruptTime < DEBOUNCE
  ) return;
  lastInterruptTime = interruptTime;

  // Read CLK and DT
  int clkState = digitalRead(ENCODER_A);
  int dtState = digitalRead(ENCODER_B);

  // Determine direction (simple quadrature logic)
  if (clkState != dtState) {
    encoderPos++;  // Clockwise
  } else {
    encoderPos--;  // Counterclockwise }
  }
}


/* 
 * reading the encoder position and preparing the jog command.
 * it is triggered every 100ms form the main loop
 */
void encoderOut() {
  if (jogAxis == "none") {
    return;
  }
  if (encoderPos == lastPos) {
    return;  // No change in position
    }
  float difference = lastPos - encoderPos;
  // Serial.println(difference);
  float mm = (difference/float(PULSES_PER_REVOLUTION))*float(MM_PER_REVOLUTION);
  if (abs(mm) > shortestMove){
    String cmd = calculateCmd(mm);
    Serial.print(encoderPos);
    Serial.println(cmd);   
    webSocket.sendTXT(cmd);
  }
  lastPos = encoderPos;
}


// builds the command with correct feedrate
String calculateCmd(float mm){
  float feed = mm * 600.0;
      String cmd = "$J=G91 ";
      cmd += jogAxis;
      cmd += mm;
      cmd += " F";
      cmd += feed;
      cmd += "\n";
  return(cmd);
}

