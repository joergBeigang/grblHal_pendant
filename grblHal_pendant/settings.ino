

void readSettings() {
  settings.joystickXCenter = prefs.getInt("joyXCenter", 1247);
  settings.joystickYCenter = prefs.getInt("joyYCenter", 1902);
  settings.joystickXMax = prefs.getInt("joyXMax", 1514);
  settings.joystickYMax = prefs.getInt("joyYMax", 52);
  settings.joystickXMin = prefs.getInt("joyXMin", 257);
  settings.joystickYMin = prefs.getInt("joyYMin", 35);
  settings.joystickInvertX = prefs.getBool("joyInvX", false);
  settings.joystickInvertY = prefs.getBool("joyInvY", false);
  settings.joystickSpeed = prefs.getFloat("joySpeed", 3.0);
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
  if (prefs.isKey("joyInvX") == false)  prefs.putBool("joyInvX", false);
  if (prefs.isKey("joyInvY") == false) prefs.putBool("joyInvY", false);
  return true;
}

void checkJoystickSettings() {
  if (verifySettings() == false) {
    currentPage = &joystickStartupPage;
  }
}
