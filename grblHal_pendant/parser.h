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
