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
#include "PSXInterface.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 2    // Connected to TX of module
#define TX_PIN 3    // Connected to RX of module
#define BAUD 115200 // Default baud rate

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

// PSX Controller pins
#define DATA 13
#define CMND 12
#define ATT  11
#define CLK  10

SoftwareSerial swSerial(RX_PIN, TX_PIN);
PSXController controller(DATA, CMND, ATT, CLK);


// Setup
void setup() {
    Serial.begin(BAUD);
    swSerial.begin(BAUD);
}


// Poll PSX controller for state, create HID command, and transmit
// over bluetooth serial connection.
void loop() {

    // Fetch PSX controller state
    PSXState* state = controller.poll(); 

    // Parse and transmit over bluetooth
    uint8_t* cmnd = HIDCommand(state);
    swSerial.write(cmnd, 8);

    // Clean-up and delay to avoid overloading HC-05
    delete state;
    delete[] cmnd;
    delay(5000);
}


// Generate the HID command seqeunce in respect to the current state of
// PSX gamepad. Returns a pointer to a dynamic array containing the 8-byte sequence.
// Ensure memory freed after use of returned value to avoid mem leaks.
uint8_t* HIDCommand(PSXState* state) {

    // General
    uint8_t* command = new uint8_t[8];
    command[0] = 0xFD;
    command[1] = 0x06;

    // Analogue stick(s)
    command[2] = 0x00;
    command[3] = 0x00;
    command[4] = 0x00; 
    command[5] = 0x00;

    // Buttons (0 - 7)
    uint8_t btnMask_1 = 0x00;
    if (state->cross)    { btnMask_1 |= BTN_CROSS;     }
    if (state->circle)   { btnMask_1 |= BTN_CIRCLE;    }
    if (state->square)   { btnMask_1 |= BTN_SQUARE;    }
    if (state->triangle) { btnMask_1 |= BTN_TRIANGLE;  }
    if (state->lb)       { btnMask_1 |= BTN_LB;        }
    if (state->rb)       { btnMask_1 |= BTN_RB;        }
    if (state->lt)       { btnMask_1 |= BTN_LT;        }
    if (state->rt)       { btnMask_1 |= BTN_RT;        }
    command[6] = btnMask_1;

    // Buttons (8- 15)
    uint8_t btnMask_2 = 0x00;
    if (state->select)   { btnMask_2 |= BTN_SELECT;    }
    if (state->start)    { btnMask_2 |= BTN_START;     }
    if (state->lthumb)   { btnMask_2 |= BTN_LTHUMB;    }
    if (state->rthumb)   { btnMask_2 |= BTN_RTHUMB;    }
    if (state->up)       { btnMask_2 |= BTN_UP;        }
    if (state->down)     { btnMask_2 |= BTN_DOWN;      }
    if (state->left)     { btnMask_2 |= BTN_LEFT;      }
    if (state->right)    { btnMask_2 |= BTN_RIGHT;     }
    command[7] = btnMask_2;
    
    return command;
}
