
#ifndef STREAM_H
#define STREAM_H

void processQueue();
void sendToGrbl(const String &cmd);
void sendSingleCommand(String cmd);

#endif
