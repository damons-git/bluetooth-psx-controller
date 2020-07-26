# Bluetooth PSX Controller

An electronics project to update a standard PlayStation 1 (PS1) dualshock controller into a HID compliant bluetooth variant.

This project was inspired by the work done by Kevin Ahrendt and Andrew J. McCubbin.

Kevin's Arduino implementation of the non-analogue PSX controller protocol can be found here, [Arduino Playground - PSXLibrary](https://playground.arduino.cc/Main/PSXLibrary/), and a detailed write of the PSX controller protocol by Andrew here, [Playstation Controller Information](https://gamesx.com/controldata/psxcont/psxcont.htm)

Note: Though code has been added to add a 'deadzone' to the thumbsticks of the analgoue controller, being a roughly 20 year old controller the thumbsticks in my case did require replacing. To do this new thumbsticks with the same footprint were purchased and soldered to the main board.
