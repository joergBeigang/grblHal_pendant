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


void readSettings() {
  settings.joystickXCenter = prefs.getInt("joyXCenter", 2048);
  settings.joystickYCenter = prefs.getInt("joyYCenter", 2048);
  settings.joystickXMax = prefs.getInt("joyXMax", 4095);
  settings.joystickYMax = prefs.getInt("joyYMax", 4095);
  settings.joystickXMin = prefs.getInt("joyXMin", 0);
  settings.joystickYMin = prefs.getInt("joyYMin", 0);
  settings.joystickInvertX = prefs.getBool("joyInvX", false);
  settings.joystickInvertY = prefs.getBool("joyInvY", false);
  settings.joystickSpeed = prefs.getFloat("joySpeed", 3.0);
  settings.joystickDeadZone = prefs.getFloat("joyDead", 0.15);
  settings.encoderInvertX = prefs.getBool("encInvX", false);
  settings.encoderInvertY = prefs.getBool("encInvY", false);
  settings.encoderInvertZ = prefs.getBool("encInvZ", false);
  settings.accCompensation = prefs.getFloat("accCmp", 0.0);
}


// startup check if values for the joystick are existing
bool verifySettings() {
  // add verification code here if needed
  if (prefs.isKey("joyXCenter") == false) return false;
  if (prefs.isKey("joyYCenter") == false) return false;
  if (prefs.isKey("joyXMax") == false) return false;
  if (prefs.isKey("joyXMin") == false) return false;
  if (prefs.isKey("joyYMax") == false) return false;
  if (prefs.isKey("joyYMin") == false) return false;
  if (prefs.isKey("joySpeed") == false)  prefs.putFloat("joySpeed", 3.0);
  if (prefs.isKey("joyDead") == false)  prefs.putFloat("joyDead", 0.15);
  if (prefs.isKey("joyInvX") == false)  prefs.putBool("joyInvX", false);
  if (prefs.isKey("joyInvY") == false) prefs.putBool("joyInvY", false);
  if (prefs.isKey("encInvX") == false) prefs.putBool("encInvX", false);
  if (prefs.isKey("encjoyInvY") == false) prefs.putBool("encInvY", false);
  if (prefs.isKey("encjoyInvZ") == false) prefs.putBool("encInvZ", false);
  if (prefs.isKey("encjoyInvZ") == false) prefs.putBool("encInvZ", false);
  if (prefs.isKey("accComp") == false)  prefs.putFloat("accCmp", 0.0);
  return true;
}

void checkJoystickSettings() {
  if (verifySettings() == false) {
    currentPage = &joystickStartupPage;
  }
}
