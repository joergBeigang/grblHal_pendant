/*
This file is part of grblHal_pendant.
grblMachinist is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

grblMachinist is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
grblMachinist.
If not, see <https://www.gnu.org/licenses/>.
*/

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
  float joystickDeadZone;
  bool encoderInvertX;
  bool encoderInvertY;
  bool encoderInvertZ;
  float accCompensation;
};

void readSettings();
void checkJoystickSettings();
bool verifySettings();
#endif
