# OpenDial

This project is ESP32 firmware for a clone of a Microsoft Surface Dial-like device. 

It implements the surface dial protocol over BLE HID and appears as a Surface Dial to Windows. This means it works with all the apps that implement support for the surface dial, like Photoshop or Sketchable.

This isn't exactly in a release-ready state yet, but a bunch of people have been asking for the code and I've been busy with other projects, so I've just dumped it here for now. All the stuff you need to implement your project is here, but it's not a in a turnkey "just press compile" state. I'm also not supporting this repository at all at the moment, so you are on your own.

Resources that may be helpful for you:
https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/radial-implementation-guide

This code also uses the BLEKeyboard library from https://github.com/T-vK/ESP32-BLE-Keyboard