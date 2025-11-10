/*
 * functions for jogging using a joystick
 * and the rotary encoder for the z axis
 */

int joyXCenter = 1284;
int joyYCenter = 1744;
float joyXMax = 0.6; // max value for X axis
float joyYMin = 0.85; // min value for Y axis

// main function to read the joystick and send jog commands
void readJoystick(){
    float valueX = prepareJoystickValue(analogRead(JOY_X_PIN), .25, .31, .1,joyXCenter, .8);
    float valueY = prepareJoystickValue(analogRead(JOY_Y_PIN), .39, .48, .1,joyYCenter, .8);
    float valueZ = readJoystickEncoder();
    float vec[3] = {valueX, valueY, valueZ};
    float mag = magnitude(vec, 3);
    String cmd = jog_build_cmd(valueX, valueY, valueZ, mag);
    if (cmd != ""){
      Serial.println(cmd);
    }
  }

// read the encoder and return the distance moved in mm
float readJoystickEncoder(){
    static int lastPos = 0;
    static int difPos = 0;

    
    if (lastPos != encoderPos) {
          // send jog command
          
          difPos = encoderPos - lastPos;
          lastPos = encoderPos;
          float mm = calculateJoystickEncoderDistance(difPos);
          return mm;
    }
    return 0.0;
}

// translate the encoder steps into mm distance
float calculateJoystickEncoderDistance(int steps){
    // one revolution is 10mm
    // 1200 steps per revolution
    Serial.println(steps);
    float neg = 1.0;
    if (steps < 0){
      neg = -1.0;
      steps = abs(steps);
    }
    return ((float(steps) / 1200.0) * 10.0) * neg;
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
    Serial.print("raw value: ");
    value = mapJoystickValue(value, minVal, maxVal, centerZone);
    value = normalizeJoystickValue((value), centerZone, 1.0);
    value = ease_in_blend(value, blend);
    Serial.println(value);
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
String jog_build_cmd(float x, float y, float z, float magnitude){
    bool chk = false;
    String cmd  = "$J=G91";
    float feed = calculateFeed(magnitude);
    float val;
    if (x != 0){
        val = calculateDistance(x, feed);
        Serial.print(val);
        cmd = cmd + " X" + String(val, 3);
        chk = true;
    }
    if (y != 0){
        val = calculateDistance(y, feed);
        cmd = cmd + " Y" + String(val, 3);
        chk = true;
    }
    if (z != 0){
        val = calculateDistance(z, feed);
        cmd = cmd + " Z";
        cmd = cmd + String(val, 3);
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

// clculate the distance to move based on feed rate and the reaction time of
// grblHal (float dt = .2)
float calculateDistance(float value, float feed){ 
    // calculate distance
    float dt = 0.2;
    float distance = feed / 60 * dt;
    if (value < 0){
        distance = distance * -1;
    }
    return distance;
}
