// set to 0 to disable all debug printing on Serial (usb)
#define DEBUG 1

// pins for uart communicatino with grblhal
#define GRBL_RX 16
#define GRBL_TX 17

// mm travel for each full ratation of the jog encoder
#define MM_PER_REVOLUTION 5
// pulses per revolution of the jog encoder
#define PULSES_PER_REVOLUTION 600
// interval for sending jog commands in ms
#define SEND_INTERVAL 100


// Jogging joyustick
#define JOY_X_PIN 2  // GP26, ADC0
#define JOY_Y_PIN 15

#define JOY_MAX_FEED 3000

// ************************
// rotray encoder pin setup
// ************************
//
// Jogging rotary encoder
// encoder for jogging
#define ENCODER_A 18
#define ENCODER_B 19

// EC11 rotary encder
// menu
#define MENU_A 26
#define MENU_B 27
#define MENU_SW 25

// feed overrides
#define FEED_A 13
#define FEED_B 12
#define FEED_SW 14

// spindle overrides
#define SPINDLE_A 33
#define SPINDLE_B 32
#define SPINDLE_SW 4

// *********************
// rotary encoder invert
// *********************
// 1 = invert is true
// 0 = invert is false
#define MENU_INVERT 1


// timer for swtiching back from UART mode to normal mode 
// after last input via joystick or encoder
// time in milli seconds
#define CANCEL_UART_TIMER 600

#if DEBUG
  #define DEBUG_PRINT(x)     Serial.print(x)
  #define DEBUG_PRINTLN(x)   Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

