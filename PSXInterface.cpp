#include <math.h>
#include "PSXStruct.h"
#include "PSXInterface.h"

// Instantiate a new PSXInterface object.
// data : pin number for data line
// cmnd : pin number for command line
// att  : pin number for attention
// clk  : pin number for clock
// deadzone : percent of analogue stick around center that is inactive, increase if drifting.
PSXController::PSXController(byte data, byte cmnd, byte att, byte clk, double deadzone = 0.0) {
    this->dataPin = data;
    this->commandPin = cmnd;
    this->attentionPin = att;
    this->clockPin = clk;
    this->deadzonePercent = deadzone;

    pinMode(data, INPUT);
    pinMode(cmnd, OUTPUT);
    pinMode(att, OUTPUT);
    pinMode(clk, OUTPUT);
    digitalWrite(data, HIGH);
    digitalWrite(cmnd, LOW);
    digitalWrite(att, HIGH);
    digitalWrite(clk, HIGH);
}


// Pull attention pin low to commence communication
// Command line 0x01 (start)
// Command line 0x42 (request) simultaneous receive on Data line 0x73 (controller type)
// Data line receive 0x5A (commmencing data)
// Data line receive 6-bytes (controller status)
//
// N.B. Clock needs to pull low during transmission of bit and back high after.
// N.B. All transmissions are read/written LSB first (little endian)
//
PSXState* PSXController::poll() {
    digitalWrite(this->attentionPin, LOW);

    // Transmit start (0x01)
    for (int i = 0; i < 8; i++) {
        int singleBit = (0x01 & (1 << i));
        if (singleBit) {
            digitalWrite(this->commandPin, HIGH);  
        } else {
            digitalWrite(this->commandPin, LOW);
        }
        
        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);
        digitalWrite(this->clockPin, HIGH);
        delayMicroseconds(50);
    }

    // Transmit data request (0x42) and receive controller type
    byte controllerType = 0x00;
    for (int i = 0; i < 8; i++) {
        int singleBit = (0x42 & (1 << i));
        if (singleBit) {
            digitalWrite(this->commandPin, HIGH);  
        } else {
            digitalWrite(this->commandPin, LOW);
        }

        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);

        int tempBit = digitalRead(this->dataPin);
        if (tempBit) {
            controllerType |= (0b10000000 >> i);
        }

        digitalWrite(this->clockPin, HIGH);
        delayMicroseconds(50);
    }

    // Ignore commening read data (0x5A received on data)
    byte commencingData = 0x00;
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);

        int tempBit = digitalRead(this->dataPin);
        if (tempBit) {
            commencingData |= (0b10000000 >> i);
        }
        
        digitalWrite(this->clockPin, HIGH);
        delayMicroseconds(50);
    }
    // Halt failed reads, re-request data.
    if (commencingData != 0x5A) {
        digitalWrite(this->attentionPin, HIGH);
        delayMicroseconds(100);
        return poll();   
    }

    // Read following six-bytes on data line
    byte resp [6] ;
    for (int i = 0; i < 6; i++) {
        byte tempByte = 0x00;
        for (int j = 0; j < 8; j++) {
            digitalWrite(this->clockPin, LOW);
            delayMicroseconds(50);
          
            int tempBit = digitalRead(this->dataPin);
            if (tempBit) {
                tempByte |= (0b10000000 >> j);
            }
 
            digitalWrite(this->clockPin, HIGH);
            delayMicroseconds(50);
        }

        resp[i] = reverseByte(tempByte);
    }

    // debug(resp); // Uncomment to display debug output
    digitalWrite(this->attentionPin, HIGH);
    delayMicroseconds(100);
    struct PSXState* state = respToState(resp);
    return state;
}


// Convert the byte response from the controller to the internal
// PSX controller struct format
PSXState* PSXController::respToState(byte* data) {
    struct PSXState* psx = new PSXState();
  
    byte buttonsOne =  data[0] ^ 0xFF;
    byte buttonsTwo =  data[1] ^ 0xFF;
    int8_t rightThumbX = (int) addDeadzone(data[2]) - 128;
    int8_t rightThumbY = (int) addDeadzone(data[3]) - 128;
    int8_t leftThumbX =  (int) addDeadzone(data[4]) - 128;
    int8_t leftThumbY =  (int) addDeadzone(data[5]) - 128;

    if (buttonsOne & SELECT)   psx->select   = true;
    if (buttonsOne & JOYRIGHT) psx->rthumb   = true;
    if (buttonsOne & JOYLEFT)  psx->lthumb   = true;
    if (buttonsOne & START)    psx->start    = true;
    if (buttonsOne & UP)       psx->up       = true;
    if (buttonsOne & RIGHT)    psx->right    = true;
    if (buttonsOne & DOWN)     psx->down     = true;
    if (buttonsOne & LEFT)     psx->left     = true;
    if (buttonsTwo & L2)       psx->lt       = true;
    if (buttonsTwo & R2)       psx->rt       = true;
    if (buttonsTwo & L1)       psx->lb       = true;
    if (buttonsTwo & R1)       psx->rb       = true;
    if (buttonsTwo & TRIANGLE) psx->triangle = true;
    if (buttonsTwo & CIRCLE)   psx->circle   = true;
    if (buttonsTwo & CROSS)    psx->cross    = true;
    if (buttonsTwo & SQUARE)   psx->square   = true;

    psx->lthumb_x = leftThumbX;
    psx->lthumb_y = leftThumbY;
    psx->rthumb_x = rightThumbX;
    psx->rthumb_y = rightThumbY;

    return psx;
}


// Reverse the bit order of a given byte
byte PSXController::reverseByte(byte data) {
    byte out = 0x00;
    for (int i = 0; i < 8; i++) {
        int mask = (int) ceil(pow(2, i));
        if (data & mask) {
          out |= 0x01 << (7 - i);
        }
    }
    return out;
}


// Print a debug output of the controller response data
void PSXController::debug(byte* data) {
    byte buttonsOne =  data[0] ^ 0xFF;
    byte buttonsTwo =  data[1] ^ 0xFF;
    uint8_t rightThumbX = data[2];
    uint8_t rightThumbY = data[3];
    uint8_t leftThumbX =  data[4];
    uint8_t leftThumbY =  data[5];

    if (buttonsOne & SELECT)   Serial.println("SELECT");
    if (buttonsOne & JOYRIGHT) Serial.println("JOYRIGHT");
    if (buttonsOne & JOYLEFT)  Serial.println("JOYLEFT");
    if (buttonsOne & START)    Serial.println("START");
    if (buttonsOne & UP)       Serial.println("UP");
    if (buttonsOne & RIGHT)    Serial.println("RIGHT");
    if (buttonsOne & DOWN)     Serial.println("DOWN");
    if (buttonsOne & LEFT)     Serial.println("LEFT");

    if (buttonsTwo & L2)       Serial.println("L2");
    if (buttonsTwo & R2)       Serial.println("R2");
    if (buttonsTwo & L1)       Serial.println("L1");
    if (buttonsTwo & R1)       Serial.println("R1");
    if (buttonsTwo & TRIANGLE) Serial.println("TRIANGLE");
    if (buttonsTwo & CIRCLE)   Serial.println("CIRCLE");
    if (buttonsTwo & CROSS)    Serial.println("CROSS");
    if (buttonsTwo & SQUARE)   Serial.println("SQUARE");

    Serial.print("Right analogue stick: ");
    Serial.print("X: " + (String) rightThumbX + " | ");
    Serial.println("Y: " + (String) rightThumbY);
    Serial.print("Left analogue stick:  ");
    Serial.print("X: " + (String) leftThumbX + " | ");
    Serial.println("Y: " + (String) leftThumbY);

    Serial.print("\n");
}


// Adds deadzone to joystick reading
// PSX analogue stick readings range from 0x00 - 0xFF.
// Dependent on the deadzone percent limit this function will
// consider some minor movements centered.
byte PSXController::addDeadzone(byte analogueReading) {
    int deadRange = (int) ceil(255 * this->deadzonePercent);
    byte centered = 0x7F; // ceil(255/2) = 127
    Serial.println(analogueReading, HEX);
    Serial.println(deadRange);

    if ((analogueReading > (centered - deadRange)) && (analogueReading < (centered + deadRange))) {
        return centered;
    } else {
        return analogueReading;  
    }
}
