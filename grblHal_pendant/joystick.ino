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
 * functions for jogging using a joystick
 * and the rotary encoder for the z axis at the same time
 *
 * 1. joystick calculation for xy based on the distacne.
 *    the value is multiplied with the speed setting. 
 * 2. the encoder is read for z axis movement
 * 3. with the magnitude of the 3d vector the feed rate is calculated
 * 
 */

int lastPosJoystick = 0;
int difPosJoystick = 0;

void initJoystick() {
  readSettings();
  resetQueue();
  // lastPosJoystick = encoderPos;
  initEncoder();
  difPosJoystick = 0;
}
// main function to read the joystick and send jog commands
void readJoystick(){
  // prepareJoystickValue returns a normalized float
  // float value, float minVal, float maxVal, float centerZone, float center, float blend
  float valueX = prepareJoystickValue(analogRead(JOY_X_PIN),
                                      settings.joystickXMin,
                                      settings.joystickXMax,
                                      settings.joystickDeadZone,
                                      settings.joystickXCenter, 
                                      .8);
  float valueY = prepareJoystickValue(analogRead(JOY_Y_PIN),
                                      settings.joystickYMin,
                                      settings.joystickYMax,
                                      settings.joystickDeadZone,
                                      settings.joystickYCenter,
                                      .8);
  // readJoystickEncoder returns a distance in mm
  float valueZ = readEncoderPos();
  // no input, do nothing.
  if (valueX == 0.0 && valueY == 0.0 && valueZ == 0.0){
    return;
  } 
  // invert axis according to settings
  if (settings.joystickInvertX == true) valueX = -valueX;
  if (settings.joystickInvertY == true) valueY = -valueY;
  if (settings.encoderInvertZ == true) valueZ = -valueZ;

  timerEncoderRest = millis();
  // input, but grblhal isn't listening, switch uart on
  if (grblStatus.uartMode == false){
    // first check if the last rt command wasn't to recent
    if (millis() - rtCmdTimer < 200) return;
    DEBUG_PRINTLN("JOYSTICK toggle uart ");
    toggleEnable();
  }

  // float vec[3] = {valueX, valueY, valueZ};
  String cmd = jog_build_cmd(valueX, valueY, valueZ);
  // DEBUG_PRINTLN(cmd);
  if (cmd != ""){
    // DEBUG_PRINTLN(cmd);
    sendToGrbl(cmd);
  }
}

// read the encoder and return the distance moved in mm
// TODO remove after testing
float readJoystickEncoder(){
  if (lastPosJoystick != encoderPos) {
    // send jog command
    
    difPosJoystick = encoderPos - lastPosJoystick;
    lastPosJoystick = encoderPos;
    float mm = calculateJoystickEncoderDistance(difPosJoystick);
    return mm;
  }
  return 0.0;
}

// translate the encoder steps into mm distance
float calculateJoystickEncoderDistance(int steps){
    // one revolution is 10mm
    // 1200 steps per revolution
    float neg = 1.0;
    if (steps < 0){
      neg = -1.0;
      steps = abs(steps);
    }
    return ((float(steps) / 1200.0) * 5.0) * neg;
}
// compute the magnitude of a 2D or 3D vector
float magnitude(const float *v, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += v[i] * v[i];
    }
    return sqrtf(sum);
}

/* 
 * pepare joystick value by mapping, normalizing and easing
 * uvalue, the raw value read by alanogRead
 * minVal, normalized max value of the joystick reading
 * maxVal, normalized min value of the joystick reading
 * centerZone, deadzone around center position
 * blend, amount of easing blend (between linear and squared - 0 - 1)
 */
float prepareJoystickValue(int ivalue, int minValRaw, int maxValRaw, float centerZone, int center, float blend){
    float centerNormalized = float(center) / 4096.0;
    float value = normalize(ivalue, minValRaw, center, maxValRaw);
    value = mapJoystickValue(value, centerZone);
    value = ease_in_blend(value, blend);
    // DEBUG_PRINT("min: ");
    // DEBUG_PRINT(minValRaw);
    // DEBUG_PRINT("max ");
    // DEBUG_PRINT(maxValRaw);
    // DEBUG_PRINT("center ");
    // DEBUG_PRINT(center);
    // DEBUG_PRINT("final ");
    // DEBUG_PRINTLN(value);
    value = clampValue(value);
    return value;
}

float clampValue(float value) {
  if (value < -1) value = -1;
  if (value > 1) value = 1;
  return value;
}
// normalize the raw (0 to 4096) joystick value based on center position
float normalize(int val, int minVal, int centerVal, int maxVal) {
    if (val < centerVal) {
        return float(val - centerVal) / float(centerVal - minVal);
    } else {
        return float(val - centerVal) / float(maxVal - centerVal);
    }
}


// normalize the joystick value in order to let it start from 0 even when 
// the center bit is cut off
float normalizeJoystickValue(float x, float min_val, float max_val){
    if (x == 0.0){
      return 0.0;
    }else if (x > 0){ 
        return ((x - min_val) / (max_val - min_val));
    } else {
        return ((x + min_val) / (max_val - min_val));
    }
}

// quadratci ease-in blending function
// to give the joystick a better control of slow movemnents
float ease_in_blend(float value, float amount){
    /*
    Ease-in function with adjustable amount.

    t: input in [0,1]
    amount: blend between linear (0) and func(x) (1)
    func: the easing curve to blend with
    */
    int neg = 1;
    if (value < 0.0){
      neg = -1;
    }
    float linear = abs(value);
    float nonlinear = value * value; // quadratic ease-in
    return ((1 - amount) * linear + amount * nonlinear) * neg;
}
// value: normalized [-1, 1]
// centerZone: deadzone fraction, e.g., 0.1
float mapJoystickValue(float value, float centerZone) {
    // deadzone
    if (fabs(value) < centerZone) {
        return 0.0f;
    }

    // scale remaining range so 0..centerZone..1 maps linearly to 0..1
    float sign = (value > 0) ? 1.0f : -1.0f;
    float scaled = (fabs(value) - centerZone) / (1.0f - centerZone);

    return scaled * sign;
}

// builds the jog command for grblHAL
String jog_build_cmd(float x, float y, float z){
  float distMultiplier = settings.joystickSpeed;
  x = x * distMultiplier;
  y = y * distMultiplier;
  float vec[3] = {x,y,z};
  float mag3d = magnitude(vec, 3);

  bool chk = false;
  String cmd  = "$J=G91";
  // float feed2d = calculateFeed(mag2d);
  const long interval = float(SEND_INTERVAL);         // interval for sending jog commands in milliseconds
  float feed = abs(mag3d * (60000/interval)) * (1 - settings.accCompensation);

  if (x != 0){
    cmd = cmd + " X" + String(x, 3);
    chk = true;
  }
  if (y != 0){
    cmd = cmd + " Y" + String(y, 3);
    chk = true;
  }
  if (z != 0){
    cmd = cmd + " Z" + String(z, 3);
    chk = true;
  }
  cmd = cmd + " F" + String(feed, 0) + "\n";
  if (chk == false){
    return "";
  }
  return cmd;
}

// calcualte the feed rade based on joystick magnitude
float calculateFeed(float value){ 
    float max_feed = float(JOY_MAX_FEED);
    return (max_feed * abs(value));
}


// ******************************
// joystick calibration functions
// ******************************

// joyCalib holds the temporary calibration values
JoystickCalibtration joystickCalib = {
  .xAxisCenter = 2048,
  .yAxisCenter = 2048,
  .xAxisMax = 4095,
  .yAxisMax = 4095,
  .xAxisMin = 0,
  .yAxisMin = 0,
};

void joyCalibrateCenter() {
  joystickCalib.xAxisCenter = joyCalibrateRead(int(JOY_X_PIN));
  joystickCalib.yAxisCenter = joyCalibrateRead(int(JOY_Y_PIN));
}

void joyCalibrateUp() {
  joystickCalib.yAxisMax = joyCalibrateRead(int(JOY_Y_PIN));
}

void joyCalibrateDown() {
  joystickCalib.yAxisMin = joyCalibrateRead(int(JOY_Y_PIN));
}

void joyCalibrateLeft() {
  joystickCalib.xAxisMin = joyCalibrateRead(int(JOY_X_PIN));
}

bool joyCalibrateRight() {
  joystickCalib.xAxisMax = joyCalibrateRead(int(JOY_X_PIN));
  calibrateJoystick();
  return sanityCheck();
}


int joyCalibrateRead(int pin) {
    const int N = 50;
    int readings[N];

    // get samples
    for (int i = 0; i < N; i++) {
        readings[i] = analogRead(pin);
        delay(2); // let joystick settle
    }
    // get median
    int sorted[N];
    memcpy(sorted, readings, sizeof(readings));
    std::sort(sorted, sorted + N);
    int median = sorted[N/2];

    Serial.print("value ");
    Serial.println(median);
    return median;
}

void calibrateJoystick() {
  // center
  prefs.putInt("joyXCenter", joystickCalib.xAxisCenter);
  prefs.putInt("joyYCenter", joystickCalib.yAxisCenter);
  // we're not sure how the joystick is wired so up is not necessarily max
  if (joystickCalib.xAxisMax > joystickCalib.xAxisMin){
    prefs.putInt("joyXMax", joystickCalib.xAxisMax);
    prefs.putInt("joyXMin", joystickCalib.xAxisMin);
  } else {
    prefs.putInt("joyXMax", joystickCalib.xAxisMin);
    prefs.putInt("joyXMin", joystickCalib.xAxisMax);
  }
  if (joystickCalib.yAxisMax > joystickCalib.yAxisMin){
    prefs.putInt("joyYMax", joystickCalib.yAxisMax);
    prefs.putInt("joyYMin", joystickCalib.yAxisMin);
  } else {
    prefs.putInt("joyYMax", joystickCalib.yAxisMin);
    prefs.putInt("joyYMin", joystickCalib.yAxisMax);
  }
}

// check if the result of the joystick calibration is valid
bool sanityCheck() {
  // DELTA is the minimum difference between min, center and max
  const int DELTA = 600;
  readSettings();
  DEBUG_PRINTLN("check1");
  if (settings.joystickXMax - settings.joystickXCenter < DELTA) return false;
  DEBUG_PRINTLN("check2");
  if (settings.joystickXCenter - settings.joystickXMin < DELTA) return false;
  DEBUG_PRINTLN("check3");
  if (settings.joystickYMax - settings.joystickYCenter < DELTA) return false;
  DEBUG_PRINTLN("check4");
  if (settings.joystickYCenter - settings.joystickYMin < DELTA) return false;
  DEBUG_PRINTLN("check passed");
  return true;
}

