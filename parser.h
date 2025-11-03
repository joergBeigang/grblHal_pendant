#ifndef GRBL_STATUS_H
#define GRBL_STATUS_H

#define MAX_AXIS 3

struct GrblStatus {
  float position_raw[MAX_AXIS];
  float position_offset[MAX_AXIS];
  float position[MAX_AXIS];
  uint8_t axis;
  char status[16];
  float feed;
  float spindle;
  float feed_ov;
  float spindle_ov;
  float rapid_ov;
  int buffer_state;
  int buffer_size;
  char g5x[8];
  float tlo;
};

// Declare the global instance
extern GrblStatus grbl;

// Declare the functions
void resetGrblStatus();
void parseGrblStatus(const String &info);

#endif
