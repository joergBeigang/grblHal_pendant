
#define MAX_AXIS 5

struct GrblStatus {
    // Machine/work coordinates
    float position_raw[MAX_AXIS];     // X Y Z A B raw
    float position_offset[MAX_AXIS];  // WCO offsets
    float position[MAX_AXIS];         // Machine - offset
    uint8_t axis;                     // number of axes

    // Status & overrides
    char status[16];
    float feed;           // FS feed
    float spindle;        // FS spindle
    float feed_ov;        // Ov feed override
    float spindle_ov;     // Ov spindle override
    float rapid_ov;       // Ov rapid override

    // Buffer
    int buffer_state;     // Bf state
    int buffer_size;      // Bf size

    // G5x & TLO
    char g5x[8];
    float tlo;
};

GrblStatus grbl;

void resetGrblStatus() {
    for (int i=0;i<MAX_AXIS;i++) {
        grbl.position_raw[i] = 0;
        grbl.position_offset[i] = 0;
        grbl.position[i] = 0;
    }
    grbl.axis = 3;
    strcpy(grbl.status, "-");
    grbl.feed = 0;
    grbl.spindle = 0;
    grbl.feed_ov = 100;
    grbl.spindle_ov = 100;
    grbl.rapid_ov = 100;
    grbl.buffer_state = 100;
    grbl.buffer_size = 1023;
    strcpy(grbl.g5x, "G54");
    grbl.tlo = 0;
}

// Helper to split string by delimiter into array of strings
void splitString(const String &s, char delim, String *out, int maxOut, int &count) {
    int lastIndex = 0;
    int nextIndex = 0;
    count = 0;
    while ((nextIndex = s.indexOf(delim, lastIndex)) != -1 && count < maxOut) {
        out[count++] = s.substring(lastIndex, nextIndex);
        lastIndex = nextIndex + 1;
    }
    if (count < maxOut && lastIndex < s.length()) {
        out[count++] = s.substring(lastIndex);
    }
}

// Parse MPos: X,Y,Z,... into position_raw and position (offset subtracted)
void parseMPos(const String &part) {
    String s = part;
    s.replace("MPos:", "");
    String fields[MAX_AXIS];
    int count;
    splitString(s, ',', fields, MAX_AXIS, count);
    grbl.axis = count;
    for (int i = 0; i < count; i++) {
        grbl.position_raw[i] = fields[i].toFloat();
        grbl.position[i] = grbl.position_raw[i] - grbl.position_offset[i];
    }
}

// Parse WCO: X,Y,Z,... into position_offset and recompute position
void parseWCO(const String &part) {
    String s = part;
    s.replace("WCO:", "");
    String fields[MAX_AXIS];
    int count;
    splitString(s, ',', fields, MAX_AXIS, count);
    for (int i = 0; i < count; i++) {
        grbl.position_offset[i] = fields[i].toFloat();
        grbl.position[i] = grbl.position_raw[i] - grbl.position_offset[i];
    }
}

// Parse Bf: buffer state,size
void parseBf(const String &part) {
    String s = part;
    s.replace("Bf:", "");
    String fields[2];
    int count;
    splitString(s, ',', fields, 2, count);
    if (count >= 1) grbl.buffer_state = fields[0].toInt();
    if (count >= 2) grbl.buffer_size = fields[1].toInt();
}

// Parse Ov: feed,spindle,rapid overrides
void parseOv(const String &part) {
    String s = part;
    s.replace("Ov:", "");
    String fields[3];
    int count;
    splitString(s, ',', fields, 3, count);
    if (count >= 1) grbl.feed_ov = fields[0].toFloat();
    if (count >= 2) grbl.spindle_ov = fields[1].toFloat();
    if (count >= 3) grbl.rapid_ov = fields[2].toFloat();
}

// Parse FS: feed, spindle speed
void parseFS(const String &part) {
    String s = part;
    s.replace("FS:", "");
    String fields[2];
    int count;
    splitString(s, ',', fields, 2, count);
    if (count >= 1) grbl.feed = fields[0].toFloat();
    if (count >= 2) grbl.spindle = fields[1].toFloat();
}

// Main status parser: expects <…> string
void parseGrblStatus(const String &info) {
    resetGrblStatus();

    int start = info.indexOf('<');
    int end = info.indexOf('>');
    if (start == -1 || end == -1) return;

    String s = info.substring(start + 1, end);
    s.toUpperCase();

    // Split by '|'
    String parts[10];
    int count;
    splitString(s, '|', parts, 10, count);
    if (count < 1) return;

    // First part is status
    parts[0].toCharArray(grbl.status, sizeof(grbl.status));

    // Parse remaining parts
    for (int i = 1; i < count; i++) {
        if (parts[i].startsWith("MPOS:")) parseMPos(parts[i]);
        else if (parts[i].startsWith("WCO:")) parseWCO(parts[i]);
        else if (parts[i].startsWith("BF:")) parseBf(parts[i]);
        else if (parts[i].startsWith("OV:")) parseOv(parts[i]);
        else if (parts[i].startsWith("FS:")) parseFS(parts[i]);
    }
}
