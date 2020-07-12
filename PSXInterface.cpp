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
    Serial.print("Reading Controller type: ");
    Serial.println(controllerType, HEX);

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
