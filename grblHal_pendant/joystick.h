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

#ifndef JOYSTICK_H
#define JOYSTICK_H

struct JoystickCalibtration {
  int xAxisCenter;
  int yAxisCenter;
  int xAxisMax;
  int yAxisMax;
  int xAxisMin;
  int yAxisMin;
};

// Declare the functions
void initJoystick();
void readJoystick();
void calibrateJoystick();
void joyCalibrateCenter();
void joyCalibrateUp();
void joyCalibrateDown();
void joyCalibrateLeft();
bool joyCalibrateRight();
float prepareJoystickValue(int value, float minVal, float maxVal, float centerZone, float center, float blend);
float readJoystickEncoder();
String jog_build_cmd(float x, float y, float z);
float calculateDistance(float value, float feed);
float calculateFeed(float value);
float mapJoystickValue(float value, float centerZone);
float ease_in_blend(float value, float amount);
float normalize(int val, int minVal, int centerVal, int maxVal);
float magnitude(const float *v, int n);
float calculateJoystickEncoderDistance(int steps);
int joyCalibrateRead(int pin);

#endif
