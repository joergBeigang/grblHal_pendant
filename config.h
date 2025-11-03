

// pins for uart communicatino with grblhal
#define GRBL_RX 16  // GP1, receive from TinyBee TX
#define GRBL_TX 17  // GP0, optional transmit to TinyBee RX

// Jogging rotary encoder
// encoder for jogging
#define ENCODER_A 18
#define ENCODER_B 19
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

