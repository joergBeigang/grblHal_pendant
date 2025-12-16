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
/*
 * functions for reading rotary encoders
 */

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
  float mm = readEncoderPos();
  if (mm != 0) {
    // check for invert
    if (invertAxis(axis) == true) mm = -mm;
    DEBUG_PRINTLN("encoder moved");
    // deal with uart mode and respect the minimum time span 
    // between sending real time commands
    timerEncoderRest = millis();
    if (grblStatus.uartMode == 0){
      DEBUG_PRINTLN("UART active");
      if (millis() - rtCmdTimer < 200) return;
      toggleEnable();
    }
    // build jog command
    String cmd = calculateCmd(mm, axis);
    // send jog command
    sendToGrbl(cmd);
  }
}


// check the settings if the axis should be inverted
bool invertAxis(String axis) {
  if (axis == "X" && settings.encoderInvertX == true) return true;
  if (axis == "Y" && settings.encoderInvertY == true) return true;
  if (axis == "Z" && settings.encoderInvertZ == true) return true;
  return false;
}


// read the encoder and return the distance moved in mm
float readEncoderPos(){

  if (lastPos!= encoderPos) {
    int difPos= encoderPos - lastPos;
    lastPos = encoderPos;
    float mm = calculateEncoderDistance(difPos);
    return mm;
  }
  return 0.0;
}


// translate the encoder steps into mm distance
float calculateEncoderDistance(int steps){
    // one revolution is 10mm
    // 1200 steps per revolution
    float neg = 1.0;
    if (steps < 0){
      neg = -1.0;
      steps = abs(steps);
    }
    return ((float(steps) / 1200.0) * 5.0) * neg;
}


// builds the command with correct feedrate
String calculateCmd(float mm, String axis){
  // float feed = abs(mm * (60000 / float(interval)))*1.2;
  float feed = abs(mm * (60000 / float(interval)));
  String cmd = "$J=G91 ";
  cmd += axis;
  cmd += String(mm, 3);
  cmd += " F";
  cmd += feed;
  cmd += "\n";
  return cmd;
}
