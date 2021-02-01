# OpenDial

This project is ESP32 firmware for a bluetooth rotary dial device. The main purpose of this device is for use with creativity and art programs, like photoshop. 

![Image of the device](https://i.redd.it/zjgpwo4b77751.jpg)

It implements the surface dial protocol over BLE HID and appears as a Surface Dial to Windows. This means it works with all the apps that implement support for the surface dial, like Photoshop or Sketchable.


Related helpful resources:
https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/radial-implementation-guide

This code also uses the BLEKeyboard library from https://github.com/T-vK/ESP32-BLE-Keyboard
