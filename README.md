# grblHal_pendant

This is a pendant for grblHal controlled cnc machines.
It is running on an ESP32. For the analog feel it is using a high resolution
rotatary encoder (with 1200 pulses per revolution), and an analog joystick for
rapid movement.
There are in total 3 EC11 rotary encoders with button functionality. Two to dial
in the feed and spindle overrides, and one for navigating the menu.
The display is a I2C OLED with 128x64 pixel resolution.
I chose those components mostly because I already had them laying around, the
rotatary encoder came out of an old pendant I made for 8bit grbl, the joystick
out of an old rc controller.
EC11 and the oled display can be ordered dirt cheap from China.

I am aware that my choice for the encoder for jogging is a little bit
unconventional, and therefore building a pandent like this is not for everyone,
but at least some code snippets might be useful for others.

All the jogging, regardless if joystick or encoder is done by reading the input
device in a frequency of 10Hz. I figured that is a good frequency to feed
jogging commands to grbl or grblHal. The revolution of the roatary encoder is
translated directly into the revolution of my ball screw, so one revolution
moves the machine be 5mm. 1200 pulses per revolution means that one pulse equals
0.004166mm movement. With the given time interval of 0.1 seconds the feed is
easy to calculate and it has the nice side effect that there is no need for any
stop jog real time commands.
This gives a pretty secure feeling when jogging.
I personally often like to touch off with a running spindle, when
preparing stock with a indexed end mill for example. And like this, this feels really
smooth.

The UART mode of grblHal is enabled, and disabled automatically. So you can switch
between the computer and pendant without pressing any buttons.

---

## Building
The code is using the arduino framework, and a few libraries:

- **U8g2lib.h** and **Wire.h** – for the display
- **Button.h** and **AiEsp32RotaryEncoder.h** – for the buttons of the EC11 encoders
- **Preferences.h** – for saving settings in the esp32
- **queue** – for streaming gcode
- **math.h**

It can be compiled and uploaded with the ardiuno ide or the arduino cli tools.

The pin assignments and the definition of the pulses per revolution of the
rotary encoder can be found in the `config.h` file.
The analog joystick needs to be calibrated first, this can be done in the menu of the pendant.
This is very important, otherwise the machine might move unexpectedly as soon as
joystick input is selected.

In the hardware folder is a schematic, and CAD files of the case. I am using
3mm alu dipond as front panel (again, because I found some in my garage). 3mm is
a pretty unusual thickness for front panels, so most likely the model need to be
adapted a little, or just 3d print a spacer that goes under the panel.

---

## Disclaimer
Use on your own risk, read the source code first, don't crash the machine or
even hurt yourself.
