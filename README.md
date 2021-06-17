# USB-Controller-to-N64

## Goal
I wanted to use my Switch Pro Controller to play on my Nintendo 64. I did not like how the Nintendo 64 controller was like, and the Switch Pro Controller was pretty comfortable to me, so I wanted a way to use it to play.

## Materials
2x Arduino Uno
1x USB Host Shield
1x N64 Controller Cable
Power adapter (at least 1)

## Libraries
USB Host Shield (https://github.com/felis/USB_Host_Shield_2.0)

## How it works
I initially tried to use a single Arduino Uno to run both the USB Host Shield and the N64 code, but I believe the N64 read/write were blocking USB data causing the controller to act like it was studdering. I figured that since I had an extra Arduino, what if I used one to only read the USB data, then have another to only read and write N64 data, it would work out. Turns out, it worked as the USB was no longer being blocked by the N64.

The USB folder contains an Arduino sketch used to read the Switch Pro Controller, then format it in the way the N64 controller protocol is formatted. This way, when the data is requested, it is the exact format we need.

The n64 folder contains an Arduino sketch used to read/write to and from the N64 console. When we need to build the N64 controller data, we can request via I2C from the other Arduino the controller data we built earlier. With only 4 bytes to send via I2C, the bus should be fast enough to not affect gameplay.

## Setup
Connect the USB Host Shield to one Arduino. Upload the USB sketch. This Arduino must be externally powered as the N64 does not provide enough power to both the Arduinos, the USB Host Shield, and the USB controller.

Upload the n64 sketch to the other Arduino. 

You will need to connect both of this Arduino's via I2C. The wiring per Arduino can be found here: https://www.arduino.cc/en/reference/wire. You will need to match SDA to SDA and SCL to SCL.

The N64 Arduino will need the data cable from the N64 console into pin 8, and the ground into ground in order to read and write data. Optionally, you can connect the power from the N64 into the Arduino VIN to power the Arduino, though it is often not enough power and may need an external power supply for this Arduino.

Once all wired up, you should be able to plug in the USB controller, and plug the N64 controller up.

## Sources
https://github.com/omgdanieltam/Wii_Classic_to_N64

https://github.com/brownan/Gamecube-N64-Controller
