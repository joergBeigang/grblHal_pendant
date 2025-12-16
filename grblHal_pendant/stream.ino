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
#include <queue>

/*
 * simple stremaing for all none real time commands being 
 * send to grblHal. 
 * In theory this shouldn't be necessary to use a queue, because we're only 
 * sending commands that are supposed to be executed within 
 * the timing, but it is the cleaner approach. 
 *
 */

std::queue<String> grblQueue;

String singleCommand = "";
int state = -1;
unsigned long t0;

void sendToGrbl(const String &cmd) {
  DEBUG_PRINTLN(cmd);
  grblQueue.push(cmd);
}


void sendBin(uint8_t hex) {
  DEBUG_PRINT("send hex");
  DEBUG_PRINTLN(hex);
  Serial2.write(hex);
}

// being triggered at max speed
void processQueue() {
  Scheduler();
  if (!grblQueue.empty()) {
    if (grblStatus.uartMode == true) {
      String cmd = grblQueue.front();   // get oldest
      Serial2.print(cmd);
      grblQueue.pop();                  // remove it
      DEBUG_PRINT("out of queue");
      DEBUG_PRINTLN(cmd);
    }
  }
}

// reset the queue after being stuck for a while
// the vaiable grblStatus.uartMode is used here. after not triggering any 
// input via encoder or joystick after one second active is set to 
// false and UART mode is being switched off. 
// after one second. No extra time needed, the switch off active timer
// can be relied on.
void resetQueue() {
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

        DEBUG_PRINTLN("schedule task1");
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
  DEBUG_PRINTLN("activate");
  if (grblStatus.uartMode == false) {
    toggleEnable();

  }
}
void task2(){
  DEBUG_PRINTLN("send command");
  DEBUG_PRINTLN(singleCommand);
  sendToGrbl(singleCommand);
}

void task3(){
  DEBUG_PRINTLN("deactivate");
  toggleEnable();
}


void sendSingleCommand(String cmd) {
  DEBUG_PRINTLN(cmd);
  singleCommand = cmd;
  state = 0;
}
