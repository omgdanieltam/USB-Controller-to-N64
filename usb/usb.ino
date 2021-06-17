#include <SwitchProUSB.h>
#include <Wire.h>
#include <string.h>

// Satisfy the IDE, which needs to see the include statement in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
SwitchProUSB SwitchPro(&Usb);
char n64[] = {0, 0, 0, 0};
uint16_t lastMessageCounter = -1;

struct AnalogData {
  float leftX;
  float leftY;
  float rightX;
  float rightY;
};

struct AnalogData analog;

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  if(Usb.Init() == -1) {
    while(1);
  }

  Wire.begin(8);
  Wire.onRequest(requestEvent);
}

void loop() {
  Usb.Task();
  SwitchPro.setLedHomeOff();

  /* DEBUGGING
  buildData();
  memset(n64, 0, 4);
  */
}

void requestEvent() {
  
  buildData();
  Wire.write(n64[0]);
  Wire.write(n64[1]);
  Wire.write(n64[2]);
  Wire.write(n64[3]);
  memset(n64, 0, 4);
  
}

void buildData() {
  Usb.Task();
  
  if (SwitchPro.connected() && lastMessageCounter != SwitchPro.getMessageCounter()) {
    lastMessageCounter = SwitchPro.getMessageCounter();
    
    // analog sticks mapping
    analog.leftX = SwitchPro.getAnalogHat(LeftHatX);
    analog.leftY = -SwitchPro.getAnalogHat(LeftHatY);
    analog.rightX = SwitchPro.getAnalogHat(RightHatX);
    analog.rightY = -SwitchPro.getAnalogHat(RightHatY);
    
    // ensure the values are within our range
    if(analog.leftX > 1500)
      analog.leftX = 1500;
    else if(analog.leftX < -1500)
      analog.leftX = -1500;
    if(analog.leftY > 1500)
      analog.leftY = 1500;
    else if(analog.leftY < -1500)
      analog.leftY = -1500;
  
    /* BYTE 1
    *  A, B, Z, Start, Dup, Ddown, Dleft, Dright
    */
    n64[0] |= SwitchPro.getButtonPress(A) << 7;
    n64[0] |= SwitchPro.getButtonPress(B) << 6;
    n64[0] |= SwitchPro.getButtonPress(ZL) << 5;
    n64[0] |= SwitchPro.getButtonPress(PLUS) << 4;
    n64[0] |= SwitchPro.getButtonPress(UP) << 3;
    n64[0] |= SwitchPro.getButtonPress(DOWN) << 2;
    n64[0] |= SwitchPro.getButtonPress(LEFT) << 1;
    n64[0] |= SwitchPro.getButtonPress(RIGHT) << 0;
  
    /* BYTE 2
     *  0, 0, L, R, Cup, Cdown, Cleft, Cright
     */
    n64[1] |= SwitchPro.getButtonPress(L) << 5;
    n64[1] |= SwitchPro.getButtonPress(ZR) << 4;
  
    if(analog.rightY > 700) {
      n64[1] |= 1 << 3;
    }
    else if(analog.rightY < -700) {
      n64[1] |= 1 << 2;
    }
    else if(analog.rightY > -700 && analog.rightY < 700) {
      n64[1] &= ~(1 << 3);
      n64[1] &= ~(1 << 2);
    }
  
    n64[1] |= SwitchPro.getButtonPress(R) << 2;
  
    if(analog.rightX < -700) {
      n64[1] |= 1 << 1;
    }
    else if(analog.rightX > 700) {
      n64[1] |= 1 << 0;
    }
    else if(analog.rightX > -700 && analog.rightX < 700) {
      n64[1] &= ~(1 << 1);
      n64[1] &= ~(1 << 0);
    }
    
    n64[1] |= SwitchPro.getButtonPress(Y) << 1;
    n64[1] |= SwitchPro.getButtonPress(X) << 0;

    analog.leftX = (int)((analog.leftX / 1500) * 80);
    analog.leftY = (int)((analog.leftY / 1500) * 80);

    if(analog.leftX > -10 && analog.leftX < 10) {
      analog.leftX = 0;
    }
    else if(analog.leftX < 0) {
      analog.leftX = analog.leftX + 10;
    }
    else if(analog.leftX > 0) {
      analog.leftX = analog.leftX - 10;
    }
    
    if(analog.leftY > -10 && analog.leftY < 10) {
      analog.leftY = 0;
    }
    else if(analog.leftY < 0) {
      analog.leftY = analog.leftY + 10;
    }
    else if(analog.leftY > 0) {
      analog.leftY = analog.leftY - 10;
    }

    n64[2] = analog.leftX;
    n64[3] = analog.leftY;
    
    /* DEBUGGING
    printBinary(n64[0]);
    Serial.print(' ');
    printBinary(n64[1]);
    Serial.print(' ');
    Serial.print(n64[2], DEC);
    Serial.print(' ');
    Serial.print(n64[3], DEC);
    Serial.print('\n');
    */
  }
}

void printBinary(byte inByte) {
  for(int b = 7; b >= 0; b--)
    Serial.print(bitRead(inByte, b));
}
