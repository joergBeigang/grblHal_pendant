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
#ifndef GRBL_STATUS_H
#define GRBL_STATUS_H

#define MAX_AXIS 3

struct GrblStatus {
  String status;
  float machinePosition[MAX_AXIS];
  float machinePositionOffsets[MAX_AXIS];
  float feedSpeed[2];
  float overRides[3];
  float position[MAX_AXIS];
  int coordinateSystem;
  bool uartMode;
};
bool parseGrblStatusReport(String report);
bool parseGrblOutput(String report);

#endif
