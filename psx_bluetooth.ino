/**
    This sketch simply interfaces with the main board of a PSX DualShock controller
    and transmits the data over a bluetooth connection. The bluetooth device is running
    the software of an RN-42 module and is set to appear as a Gamepad Human Interface Device (HID).

    The RN-42 module requires the following 8 byte sequence to transmit the state of the Gamepad.
    0xFD : Raw report descriptor
    0x06 : Data length (6 bytes)
    0x00 : X position of left analogue stick (range: -127 to 127)  [Uses two's complement]
    0x00 : Y position of left analogue stick (range: -127 to 127)  [Uses two's complement]
    0x00 : X position of right analogue stick (range: -127 to 127) [Uses two's complement]
    0x00 : Y position of right analogue stick (range: -127 to 127) [Uses two's complement]
    0x00 : Mask of 8-buttons (range: B0 - B7 inc.)  [e.g. 0x01 means B0 active]
    0x00 : Mask of 8-buttons (range: B8 - B15 inc.) [e.g. 0x03 means B8, B9 active]

    By default this sketch uses a software serial connection.
    To change to hardware serial replace swSerial references to Serial and use
    RX/TX pins on board to TX/RX on HC-05 respectively.

    N.B. HC-05 bluetooth module uses 3.3v logic, ensure a logic-level converter is present
    if interfacing with a 5v device (e.g. Arduino Uno).
*/

#include "PSXStruct.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 2 // Connected to TX of module
#define TX_PIN 3 // Connected to RX of module

// First byte of button(s) masks
#define BTN_CROSS    0x01 // (0000 0001)
#define BTN_CIRCLE   0x02 // (0000 0010)
#define BTN_SQUARE   0x04 // (0000 0100)
#define BTN_TRIANGLE 0x08 // (0000 1000)
#define BTN_LB       0x10 // (0001 0000)
#define BTN_RB       0x20 // (0010 0000)
#define BTN_LT       0x40 // (0100 0000)
#define BTN_RT       0x80 // (1000 0000)

// Second bye of button(s)
#define BTN_SELECT   0x01 // (0000 0001)
#define BTN_START    0x02 // (0000 0010)
#define BTN_LTHUMB   0x04 // (0000 0100)
#define BTN_RTHUMB   0x08 // (0000 1000)
#define BTN_UP       0x10 // (0001 0000)
#define BTN_DOWN     0x20 // (0010 0000)
#define BTN_LEFT     0x40 // (0100 0000)
#define BTN_RIGHT    0x80 // (1000 0000)

SoftwareSerial swSerial(RX_PIN, TX_PIN);
struct PSXState state;


void setup() {
    Serial.begin(115200);
    swSerial.begin(115200);
    Serial.println("Loaded module.");
}

void loop() {
    uint8_t command1[8] = {0xFD, 0x06, 0x90, 0x90, 0x90, 0x90, 0x01, 0x01};
    swSerial.write(command1, sizeof(command1));
    delay(5000);

    uint8_t command2[8] = {0xFD, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    swSerial.write(command2, sizeof(command2));
    delay(5000);
}
