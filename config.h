

// pins for uart communicatino with grblhal
#define PICO_RX 1   // GP1, receive from TinyBee TX
#define PICO_TX 0   // GP0, optional transmit to TinyBee RX

// Jogging
// encoder for jogging
#define ENCODER_A 18
#define ENCODER_B 19
// mm travel for each full ratation of the jog encoder
#define MM_PER_REVOLUTION 5
// pulses per revolution of the jog encoder
#define PULSES_PER_REVOLUTION 600
