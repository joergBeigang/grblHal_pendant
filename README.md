grblHal_pendant

This is a pendant for grblHal controlled cnc machines.
It is running on a ESP32. For the analog feel it is using a high resolution
rotatary encoder (with 1200 pulses per revolution), and an analog joystick for
rapid movement.
There are in total 3 EC11 rotary encoders with button functionality. Two to dial
in the feed and spindle overrides, and one for navigating the menu.
The display is a I2C OLED with 128x64 pixel resolution.
I chose those components mostly because I already had them laying around, the
rotatary encoder came out of an old pendant I made for 8bit grbl, the joystick
out of an old rc controll.
EC11 and the oled display can be ordered dirt cheap from chine.
