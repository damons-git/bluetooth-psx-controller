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
#include "BatteryMonitor.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define RX_PIN 2    // Connected to TX of module
#define TX_PIN 3    // Connected to RX of module
#define BAUD 115200 // Default baud rate

#define BATTERY_LED 5 // Battery indicator neopixel pin
#define BATTERY_PIN A0;

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
#define DATA 6
#define CMND 8
#define ATT  7
#define CLK  9

SoftwareSerial swSerial(RX_PIN, TX_PIN);
PSXController controller(DATA, CMND, ATT, CLK, 0.15);
Adafruit_NeoPixel strip(1, BATTERY_LED, NEO_GRB + NEO_KHZ800);

// Battery Control Variables (inc. hardware timer)
// Timer executed once every 4s
BatteryMonitor battery;
volatile boolean updateBatteryFlag = false;
const uint16_t t1_load = 0;
const uint16_t t1_comp = 62500;
const uint16_t prescalar = 1024;  // Set as 101 in TCCR1B
const int timerIter = 15;         // Set at 15 to execute ~ once per minute (4s * 15)
int timerCount = 0;


// Setup
void setup() {
    // Serial communcation setup
    Serial.begin(BAUD);
    swSerial.begin(BAUD);

    // Neopixel indicator setup
    strip.begin();
    strip.setBrightness(10);
    strip.show();

    // Battery update timer setup
    cli();
    TCCR1A = 0;               // Reset timer control Reg A    
    TCCR1B |=  (1 << CS12);   // Set Prescalar value (101 for 1024-bit)
    TCCR1B &= ~(1 << CS11);
    TCCR1B |=  (1 << CS10);
    TCNT1 = t1_load;          // Reset Timer 1
    OCR1A = t1_comp;          // Set output compare register
    TIMSK1 = (1 << OCIE1A);   // Set timer 1 compare bit
    sei();                    // Enable global interrupts
}


// Poll PSX controller for state, create HID command, and transmit
// over bluetooth serial connection.
void loop() {

//    int bat = battery.chargeRemaining();
//    if (bat >= 65) {
//        strip.setPixelColor(0, 0, 255, 0);       
//    }
//    else if (bat >= 35) {
//        strip.setPixelColor(0, 255, 255, 0);
//    }
//    else {
//        strip.setPixelColor(0, 255, 0, 0);
//    }
//    strip.show();

    int bat = battery.chargeRemaining();
    if (updateBatteryFlag) { 
        updateBatteryFlag = false;
        timerCount += 1;
        if (timerCount == timerIter) {
            timerCount = 0;
            strip.setPixelColor(0, 0, 255, 0);
            strip.show();
        } else {
            strip.setPixelColor(0, 0, 0, 0);
            strip.show();  
        }
    }

    // Fetch PSX controller state
    PSXState* state = controller.poll();

    // Parse and transmit over bluetooth
    uint8_t* cmnd = HIDCommand(state);
    swSerial.write(cmnd, 8);

    // Clean-up and delay to avoid overloading HC-05
    delete state;
    delete[] cmnd;
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
    command[2] = state->lthumb_x;
    command[3] = state->lthumb_y;
    command[4] = state->rthumb_x;
    command[5] = state->rthumb_y;

    // Buttons (0 - 7)
    uint8_t btnMask_1 = 0x00;
    if (state->cross)    {
        btnMask_1 |= BTN_CROSS;
    }
    if (state->circle)   {
        btnMask_1 |= BTN_CIRCLE;
    }
    if (state->square)   {
        btnMask_1 |= BTN_SQUARE;
    }
    if (state->triangle) {
        btnMask_1 |= BTN_TRIANGLE;
    }
    if (state->lb)       {
        btnMask_1 |= BTN_LB;
    }
    if (state->rb)       {
        btnMask_1 |= BTN_RB;
    }
    if (state->lt)       {
        btnMask_1 |= BTN_LT;
    }
    if (state->rt)       {
        btnMask_1 |= BTN_RT;
    }
    command[6] = btnMask_1;

    // Buttons (8- 15)
    uint8_t btnMask_2 = 0x00;
    if (state->select)   {
        btnMask_2 |= BTN_SELECT;
    }
    if (state->start)    {
        btnMask_2 |= BTN_START;
    }
    if (state->lthumb)   {
        btnMask_2 |= BTN_LTHUMB;
    }
    if (state->rthumb)   {
        btnMask_2 |= BTN_RTHUMB;
    }
    if (state->up)       {
        btnMask_2 |= BTN_UP;
    }
    if (state->down)     {
        btnMask_2 |= BTN_DOWN;
    }
    if (state->left)     {
        btnMask_2 |= BTN_LEFT;
    }
    if (state->right)    {
        btnMask_2 |= BTN_RIGHT;
    }
    command[7] = btnMask_2;

    return command;
}


// Interrupt Routine to set flag that battery state requires updating.
// N.B. Update of battery is performed in the main loop.
ISR(TIMER1_COMPA_vect) {
    TCNT1 = t1_load; // Reset counter to zero
    updateBatteryFlag = true;
}
