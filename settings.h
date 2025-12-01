
#ifndef SETTINGS_H
#define SETTINGS_H


struct Settings {
  int joystickXCenter;
  int joystickYCenter;
  int joystickXMax;
  int joystickYMax;
  int joystickXMin;
  int joystickYMin;
  int joystickInvertX;
  int joystickInvertY;
  float joystickSpeed;
};

void readSettings();
void checkJoystickSettings();
bool verifySettings();
#endif
