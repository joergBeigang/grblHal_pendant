
#ifndef SETTINGS_H
#define SETTINGS_H


struct Settings {
  int joystickXCenter;
  int joystickYCenter;
  int joystickXMax;
  int joystickYMax;
  int joystickXMin;
  int joystickYMin;
  bool joystickInvertX;
  bool joystickInvertY;
  float joystickSpeed;
  bool encoderInvertX;
  bool encoderInvertY;
  bool encoderInvertZ;
  float accCompensation;
};

void readSettings();
void checkJoystickSettings();
bool verifySettings();
#endif
