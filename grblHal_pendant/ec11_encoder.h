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
extern float valueEditIncrement;
extern float valueEditMin;
extern float valueEditMax;
#endif
