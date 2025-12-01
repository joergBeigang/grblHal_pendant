
#ifndef EC11_ENCODER_H
#define EC11_ENCODER_H

void initRotaryEncoders();
void overRideSwitches();
void rotaryMenuLoop();
void rotaryFeedLoop();
void rotarySpindleLoop();
void setValue();
void moveCursor();

struct Overrides {
  int difference;
  uint8_t cmdOne;
  uint8_t cmdTen;
  uint8_t cmdOneMinus;
  uint8_t cmdTenMinus;
};
#endif
