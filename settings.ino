

void readSettings() {
  settings.joystickXCenter = prefs.getFloat("joyXCenter", 1247);
  settings.joystickYCenter = prefs.getFloat("joyYCenter", 1902);
  settings.joystickXMax = prefs.getFloat("joyXMax", 1514);
  settings.joystickYMax = prefs.getFloat("joyYMax", 0.52);
  settings.joystickXMin = prefs.getFloat("joyXMin", 257);
  settings.joystickYMin = prefs.getFloat("joyYMin", 0.35);
  settings.joystickSpeed = prefs.getFloat("joySpeed", 5.0);
}

