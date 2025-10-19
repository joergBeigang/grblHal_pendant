
// encoder pins for high ressuolution jog encoder
#define ENCODER_A 25  // CLK (GPIO25, right side)
#define ENCODER_B 32  // DT (GPIO32, left side)
// mm travel for each full ratation of the jog encoder
#define MM_PER_REVOLUTION 5
// pulses per revolution of the jog encoder
#define PULSES_PER_REVOLUTION 600
// debouncde time for the encoder in micro seconds
// too low values will cause tiny moves after stooping the rotation
// too high values will skipt steps when turning fast
#define DEBOUNCE 200  // debounce time in micro seconds

// wifi credentials
const char* ssid = "grbl_hal";
const char* password = "57402117533679114435";


// jog configuration
float shortestMove = 0.005;  // the shortest move that is being sent via jog
const long interval = 100;  // interval for sending jog commands in milliseconds
