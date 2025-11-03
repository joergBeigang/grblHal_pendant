#include "config.h"
#include "encoder.h"
#include "parser.h"

volatile int active = 0;           // Volatile for ISR access
const int buttonPin = 20;  
  
float joyXCenter = 0.62; // center value for X axis
float joyYCenter = 0.62; // center value for Y axis
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
    int xValueRaw = analogRead(JOY_X_PIN);
    int yValue = analogRead(JOY_y_PIN);
    float xValue = (float(xValueRaw) / 2048) - joyXCenter;
    float xValueMapped = mapJoystickValue(xValue, 0.6, 0.85, 0.15);
    float xValueNormalized = normalize((xValueMapped), 0.15, 1.0);
    float xValueEased = ease_in_blend(xValueNormalized, 0.8);
    Serial.print("mapped");
    Serial.println(xValueMapped);
    Serial.print("normalized");
    Serial.println(xValueNormalized);
    
    Serial.print("eased");
    Serial.println(xValueEased);


  }

// normalize the joystick value in order to let it start from 0 even when 
// the center bit is cut off
float normalize(float x, float min_val, float max_val){
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
