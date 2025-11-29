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

String singleCommand = "";
int state = -1;
unsigned long t0;

void sendToGrbl(const String &cmd) {
    grblQueue.push(cmd);
}

// being triggered at max speed
void processQueue() {
  Scheduler();
  if (!grblQueue.empty()) {
    if (ok == true && active == true) {
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


/*
 * to send single commands UART mode must be activated before via real time 
 * command. to make sure the real time commands are not being sent at a too 
 * high frequency (which will crash grblHal) switching on uart, sending the 
 * command and switching off uart, are being Scheduled with a some delay.
 */

void Scheduler() {
  unsigned long now = millis();
  switch(state) {
    case 0:
      if (now - t0 >= 200) {

        Serial.println("schedule task1");
        task1();
        state ++;
        t0 = millis();
      }
      break;

    case 1:
      if (now - t0 >= 200) {
        task2();
        state ++;
        t0 = millis();
      }
      break;

    case 2:
      if (now - t0 >= 500) {
        task3();
        state = -1;
        t0 = millis();
      }
      break;
  }
} 

void task1(){
  Serial.println("activate");
  if (active == false) {
    toggleEnable();
    active = true;

  }
}
void task2(){
  Serial.println("send command");
  Serial.println(singleCommand);
  sendToGrbl(singleCommand);
}

void task3(){
  Serial.println("deactivate");
  toggleEnable();
  active = false;
}


void sendSingleCommand(String cmd) {
  Serial.println(cmd);
  singleCommand = cmd;
  state = 0;
}
