#include <math.h>
#include "PSXStruct.h"
#include "PSXInterface.h"

PSXController::PSXController(byte data, byte cmnd, byte att, byte clk) {
    this->dataPin = data;
    this->commandPin = cmnd;
    this->attentionPin = att;
    this->clockPin = clk;

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
// N.B. 
//
PSXState* PSXController::poll() {
    struct PSXState* state = new PSXState();
    digitalWrite(this->attentionPin, LOW);

    // Transmit start (0x01)
    Serial.print("Transmitting start byte (0x01 - 0b00000001): ");
    for (int i = 0; i < 8; i++) {
        int singleBit = (0x01 & (1 << i));
        Serial.print(singleBit);
        Serial.print("|");
        if (singleBit) {
            digitalWrite(this->commandPin, HIGH);  
        } else {
            digitalWrite(this->commandPin, LOW);
        }
        
        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);
        digitalWrite(this->clockPin, HIGH);
    }
    Serial.print("\n");

    // Transmit data request (0x42) and receive controller type
    Serial.print("Transmitting data reqest byte (0x42 - 0b01000010): ");
    byte controllerType = 0x00;
    for (int i = 0; i < 8; i++) {
        int singleBit = (0x42 & (1 << i));
        Serial.print(singleBit);
        Serial.print("|");
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
    }
    Serial.print("\n");
    Serial.print("Reading controller type: 0x");
    Serial.println(reverseByte(controllerType), HEX);

    // Ignore commening read data (0x5A received on data)
    Serial.print("Reading commencing data (0x5A - 0b01011010): ");
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);
        Serial.print(digitalRead(this->dataPin));
        Serial.print("|");
        digitalWrite(this->clockPin, HIGH);
    }
    Serial.print("\n");

    // Read following six-bytes on data line
    byte data [6] ;
    for (int i = 0; i < 6; i++) {
        byte tempByte = 0x00;
        for (int j = 0; j < 8; j++) {
            digitalWrite(this->clockPin, LOW);
          
            int tempBit = digitalRead(this->dataPin);
            if (tempBit) {
            // if (tempBit == 0) {
                tempByte |= (0b10000000 >> j);
            }
 
            digitalWrite(this->clockPin, HIGH);
        }

        data[i] = reverseByte(tempByte) ^ 0xFF;
        Serial.println(data[i]);
    }

    debug(data);

    digitalWrite(this->attentionPin, HIGH);
    return state;
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

void PSXController::debug(byte* data) {
    byte buttonsOne =  data[0];
    byte buttonsTwo =  data[1];
    byte rightThumbX = data[2];
    byte rightThumbY = data[3];
    byte leftThumbX =  data[4];
    byte leftThumbY =  data[5];

    Serial.print("\n");
    Serial.println("Debug Output:");
    Serial.println("-------------");

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
}
