#include <math.h>
#include "config.h"
#include "encoder.h"
#include "parser.h"

volatile int active = 0;           // Volatile for ISR access
const int buttonPin = 20;  
  
int joyXCenter = 1284;
int joyYCenter = 1744;
float joyXMax = 0.6; // max value for X axis
float joyYMin = 0.85; // min value for Y axis
  
  
void setup() {
  // Serial
  // Serial usb for debugging
  Serial.begin(115200);
  // Serial uart for grblHAL communication
  Serial2.begin(115200, SERIAL_8N1, GRBL_RX, GRBL_TX);

  // read joystick
  analogReadResolution(12);

  // Rotary encoder for Jogging
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  lastA = digitalRead(ENCODER_A);
  lastB = digitalRead(ENCODER_B);
  // interrupts for encoder reading
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
}

  // send real time command 0x8B to enable/disable uart command
  void toggleEnable(){
    uint8_t byteToSend = 0x8B;
    Serial2.write(byteToSend); // sends exactly one byte
    Serial.write(byteToSend); // sends exactly one byte
    Serial.println("hex");
  }


  
void readJoystick(){
    float valueX = prepareJoystickValue(analogRead(JOY_X_PIN), .25, .31, .1,joyXCenter, .8);
    float valueY = prepareJoystickValue(analogRead(JOY_Y_PIN), .39, .48, .1,joyYCenter, .8);
    float vec[3] = {valueX, valueY, 0.0};
    float mag = magnitude(vec, 3);
    String cmd = jog_build_cmd(valueX, valueY, 0.0, mag);
    Serial.println(cmd);

    Serial.print("X");
    Serial.println(valueX);
    Serial.print("Y");
    Serial.println(valueY);


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
    return ((1 - amount) * linear + amount * nonlinear) * -1;
}

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



String jog_build_cmd(float x, float y, float z, float magnitude){
    String cmd  = "$J=G91";
    float feed = calculateFeed(magnitude);
    float val;
    if (x != 0){
        val = calculateDistance(x, feed);
        cmd = cmd + " X" + String(val, 3);
    }
    if (y != 0){
        val = calculateDistance(y, feed);
        cmd = cmd + " Y" + String(val, 3);
    }
    if (z != 0){
        val = calculateDistance(y, feed);
        cmd = cmd + " Z" + String(val, 3);
    }

    cmd = cmd + " F" + String(feed, 0) + "\n";

    return cmd;
}

float calculateFeed(float value){ 
    float max_feed = float(JOY_MAX_FEED);
    return (max_feed * abs(value));
}

float calculateDistance(float value, float feed){ 
    // calculate distance
    float dt = 0.2;
    float distance = feed / 60 * dt;
    if (value < 0){
        distance = distance * -1;
    }
    return distance;
}

void loop() {
  
  // readJogEncoder();
  // String test = "<Run,MPos:1.111,2.222,3.333,WPos:4.444,5.555,6.666>";
  // parseGrblStatus(test);
  // Serial.println(grbl.position_raw[0]);
  // String state = parseGrblState(test);
  // Serial.println(state);
  readJoystick();

  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);  // forward UART data to USB Serial
  }
  delay(100);
}
