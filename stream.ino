#include <queue>

/*
 * simple stremaing for all none real time commands being 
 * send to grblHal. 
 * In theory this shouldn't be necessary because we're only 
 * sending commands that are supposed to be executed within 
 * the timing, but it is the cleaner approach. 
 * It is using a simple send and wait for ok algorythm. The ok boolean 
 * is being set in the main file where UART is being read.
 */

std::queue<String> grblQueue;

void sendToGrbl(const String &cmd) {
    grblQueue.push(cmd);
}

// being triggered at max speed
void processQueue() {
    if (!grblQueue.empty()) {
      if (ok == true) {
        ok = false;
        String cmd = grblQueue.front();   // get oldest
        Serial2.print(cmd);
        grblQueue.pop();                  // remove it
      }
    }
}

// reset the queue after being stuck for a while
// the vaiable active is used here. after not triggering any 
// input via encoder or joystick after one second active is set to 
// false and UART mode is being switched off. 
// after one second. No extra time needed, the switch off active timer
// can be relied on.
void resetQueue() {
  if (active == false && ok == false) {
    clearQueue();
    ok = true;
  }
}

// clear the queue
void clearQueue() {
    while (!grblQueue.empty()) {
        grblQueue.pop();
    }
}

