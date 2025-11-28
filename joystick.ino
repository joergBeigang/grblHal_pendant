/*
 * functions for jogging using a joystick
 * and the rotary encoder for the z axis at the same time
 *
 * 1. joystick calculation for xy based on gnea grbl wiki
 *  result is a feed rate and a distance
 *  feed is based on the joystick input
 * 2. we got a vector 2d in mm and a feed rate
 *  z axis in mm comes in play:
 *    calcuate the feed for only z motion
 *    feed of xy and feed of z move form a vector. 
 *    magnitude of this vector is the final feed rate
 */

int joyXCenter = 1284;
int joyYCenter = 1744;
float joyXMax = 0.6; // max value for X axis
float joyYMin = 0.85; // min value for Y axis
int lastPosJoystick = 0;
int difPosJoystick = 0;
long timerJoystickRest = 0;
void initJoystick() {
  lastPosJoystick = 0;
  difPosJoystick = 0;
}
// main function to read the joystick and send jog commands
void readJoystick(){

  // prepareJoystickValue returns a normalized float
  // float value, float minVal, float maxVal, float centerZone, float center, float blend
  float valueX = prepareJoystickValue(analogRead(JOY_X_PIN), .25, .31, .1,joyXCenter, .8);
  float valueY = prepareJoystickValue(analogRead(JOY_Y_PIN), .39, .48, .1,joyYCenter, .8);
  // readJoystickEncoder returns a distance in mm
  float valueZ = readJoystickEncoder();

  // no input, switch off uart listening of grblhal
  if (valueX == 0.0 && valueY == 0.0 && valueZ == 0.0){
    long currentMillis = millis();
    if (currentMillis - timerJoystickRest >= 1000) {
      if (active == true){
        active = false;
        toggleEnable();
      } 
    } 
    return;
  } 
  timerEncoderRest = millis();
  // input, but grblhal isn't listening, switch uart on
  if (active == false){
    toggleEnable();
    active = true;
  }

  // float vec[3] = {valueX, valueY, valueZ};
  float vec[2] = {valueX, valueY};
  float mag2d = magnitude(vec, 2);
  String cmd = jog_build_cmd(valueX, valueY, valueZ, mag2d);
  if (cmd != ""){
    Serial.println(cmd);
    // Serial2.print(cmd);
    sendToGrbl(cmd);
  }
}

// read the encoder and return the distance moved in mm
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
float prepareJoystickValue(float value, float minVal, float maxVal, float centerZone, float center, float blend){
    float centerNormalized = float(center) / 4096;
    value= (float(value) / 4096) - centerNormalized;
    value = mapJoystickValue(value, minVal, maxVal, centerZone);
    value = normalizeJoystickValue((value), centerZone, 1.0);
    value = ease_in_blend(value, blend);
    return value;

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

// maps the joystick value to -1.0 to 1.0 range considering deadzone
float mapJoystickValue(float value, float minVal, float maxVal, float centerZone){
    int neg = 1;
    float factor = 1.0 / maxVal;
    if (value < 0) {
      neg = -1;
      factor = 1.0 / minVal;
    }

    float result = fabs(value) * factor;
    if (result < centerZone) {
        return 0.0f;
    }
    return (result * neg);
    }

// builds the jog command for grblHAL
String jog_build_cmd(float x, float y, float z, float mag2d){
  bool chk = false;
  String cmd  = "$J=G91";
  float feed2d = calculateFeed(mag2d);
  float feed;
  float feedZ;
  float vector3d[3] = {0,0,z};

  Serial.print(x);
  Serial.print("  ");
  Serial.print(y);
  Serial.print("  ");
  Serial.println(z);
  float val;
  if (x != 0){
    val = calculateDistance(x, feed2d);
    Serial.println(x);
    Serial.println(val);

    vector3d[0] = val;
    cmd = cmd + " X" + String(val, 3);
    chk = true;
  }
  if (y != 0){
    val = calculateDistance(y, feed2d);
    vector3d[1] = val;
    cmd = cmd + " Y" + String(val, 3);
    chk = true;
  }
  // if (z != 0){
  //   cmd = cmd + " Z";
  //   cmd = cmd + String(z, 3);
  //   chk = true;
  //   feedZ = abs(z * (60000 / float(SEND_INTERVAL)));
  //   if (feed2d != 0){
  //     // feed2d and feedZ from a vector, the magnitude is the new feed rate
  //     float vec[2] = {feed2d, feedZ};
  //     feed = magnitude(vec, 2);
  //   } else { // only z movemnent - feed needs to be calculated
  //     feed = feedZ;
  //   }
  // }else { // no z movemnent feed2d is the way to go
  //   feed = feed2d;
  // }


  cmd = cmd + " F" + String(feed2d, 0) + "\n";
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

// clculate the distance to move based on feed rate and the reaction time of
// grblHal (float dt = .2)
float calculateDistance(float value, float feed){ 
    // calculate distance
    float dt = 0.2;
    float distance = (feed / 60) * dt;
    if (value < 0){
        distance = distance * -1;
    }
    return distance;
}



