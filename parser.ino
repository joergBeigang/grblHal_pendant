// String grblStatus = "";

struct GrblStatus {
  String status;
  float machinePosition[MAX_AXIS];
  float machinePositionOffsets[MAX_AXIS];
  float feedSpeed[2];
  float overRides[3];
  float position[MAX_AXIS];
};

GrblStatus grblStatus;
void parseGrblState(const String &info) {
    int start = info.indexOf('<');
    int end = info.indexOf('>');
    String s = info.substring(start + 1, end);
    int comma = s.indexOf('|');
    grblStatus.status = s.substring(0, comma);
}
  
float* parseGrblMachinePosition(const String &info, const String indicator, int size) {
    static float values[MAX_AXIS];
    int mposIndex = info.indexOf(indicator);
    if (mposIndex == -1) {
        return nullptr; // MPos not found
    }
    int start = mposIndex + indicator.length();
    // int start = mposIndex + 5; // Move pas "MPos:"
    int end = info.indexOf(',', start);
    for (int i = 0; i < size; i++) {
        int nextComma = info.indexOf(',', start);
        String value;
        if (nextComma == -1) {
            value = info.substring(start);
        } else {
            value = info.substring(start, nextComma);
        }
        // grblStatus.machinePosition[i] = value.toFloat();
        values[i] = value.toFloat();
        start = nextComma + 1;
        if (nextComma == -1) {
            break; // No more values
        }
    }
  return values;
  }

void calculatePosition(){
  grblStatus.position[0] = grblStatus.machinePosition[0] - grblStatus.machinePositionOffsets[0];
  grblStatus.position[1] = grblStatus.machinePosition[1] - grblStatus.machinePositionOffsets[1];
  grblStatus.position[2] = grblStatus.machinePosition[2] - grblStatus.machinePositionOffsets[2];
}

void parseGrblStatusReport(String report){
    // is it a status report?
    int start = report.indexOf('<');
    int end = report.indexOf('>');
    if (start == -1 || end == -1){
      return;
    }

    // all upper case 
    report.toUpperCase();

    // parse state
    parseGrblState(report);

    // parse machine position
    float* pos = parseGrblMachinePosition(report, "MPOS:", MAX_AXIS);
    if (pos != nullptr){
        for (int i = 0; i < MAX_AXIS; i++){
          grblStatus.machinePosition[i] = pos[i];
        }
    }
    pos = parseGrblMachinePosition(report, "WCO:", MAX_AXIS);
    if (pos != nullptr){
        for (int i = 0; i < MAX_AXIS; i++){
          grblStatus.machinePositionOffsets[i] = pos[i];
        }
    }

    pos = parseGrblMachinePosition(report, "FS:", 2);
    if (pos != nullptr){
        for (int i = 0; i < 2; i++){
          grblStatus.feedSpeed[i] = pos[i];
        }
    }

    pos = parseGrblMachinePosition(report, "OV:", 3);
    if (pos != nullptr){
        for (int i = 0; i < 3; i++){
          grblStatus.overRides[i] = pos[i];
        }
    }

    calculatePosition();

    Serial.print("raw: ");
    Serial.print(grblStatus.machinePosition[0]);
    Serial.print(grblStatus.machinePosition[1]);
    Serial.println(grblStatus.machinePosition[2]);
    Serial.print("wco: ");
    Serial.print(grblStatus.machinePositionOffsets[0]);
    Serial.print(grblStatus.machinePositionOffsets[1]);
    Serial.println(grblStatus.machinePositionOffsets[2]);
    Serial.print(" calculatePosition: ");
    Serial.print(grblStatus.position[0]);
    Serial.print(grblStatus.position[1]);
    Serial.println(grblStatus.position[2]);


}

