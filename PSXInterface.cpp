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
// N.B. Clock needs to pull low during transmission of bit and back high after
PSXState* PSXController::poll() {
    struct PSXState* state = new PSXState();
    digitalWrite(this->attentionPin, LOW);


    // Transmit start (0x01)
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->clockPin, LOW);

        delayMicroseconds(50);
        int singleBit = (0x01 & (1 << i));
        if (singleBit) {
            digitalWrite(this->commandPin, HIGH);  
        } else {
            digitalWrite(this->commandPin, LOW);
        }

        digitalWrite(this->clockPin, HIGH);
    }

    // Transmit data request (0x42)
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->clockPin, LOW);

        delayMicroseconds(50);
        int singleBit = (0x42 & (1 << i));
        if (singleBit) {
            digitalWrite(this->commandPin, HIGH);  
        } else {
            digitalWrite(this->commandPin, LOW);
        }

        digitalWrite(this->clockPin, HIGH);
    }

    // Ignore commening read data (0x5A received on data)
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->clockPin, LOW);
        delayMicroseconds(50);
        digitalWrite(this->clockPin, HIGH);
    }

    // Read following six-bytes on data line
    for (int i = 0; i < 6; i++) {
        byte tempByte = 0x00;
        for (int j = 0; j < 8; j++) {
            digitalWrite(this->clockPin, LOW);
          
            int tempBit = digitalRead(this->dataPin);
            if (tempBit) {
                tempByte |= (0b10000000 >> j);
            }
 
            digitalWrite(this->clockPin, HIGH);
        }
        Serial.println(tempByte, HEX);
    }

    digitalWrite(this->attentionPin, HIGH);
    return state;
}
