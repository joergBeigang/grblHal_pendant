

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

bool parseGrblStatusReport(String report){
    // is it a status report?

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
    return true;
}

int parseCoordinateSystem(String report) {

    int index = report.indexOf("G5");
    if (index == -1) {
        return 9; // MPos not found
    }
    int start = index + 1;
    // int start = mposIndex + 5; // Move pas "MPos:"
    String co = report.substring((index +1),(index+3) );
    return co.toInt();

}

bool parseGrblOutput(String report) {
    // status report check
    int start = report.indexOf('<');
    int end = report.indexOf('>');
    if (start != -1 || end != -1){
      return parseGrblStatusReport(report);
    }
    // ok check
    if (report.indexOf("ok") != -1) {
      ok = true;
    }
    // coordinate system
    start = report.indexOf('[');
    end = report.indexOf(']');
    if (start != -1 || end != -1){
      grblStatus.coordinateSystem = parseCoordinateSystem(report);
    }
  return false;

}
