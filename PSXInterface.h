/**
 * PSX Controller Interface.
 * 
 * A detailed description of the communication protocol was written about by Andrew J. McCubbin
 * read more about it at the following address:
 * https://gamesx.com/controldata/psxcont/psxcont.htm
 * 
 * 
 */

#ifndef PSX_INTERFACE_H
#define PSX_INTERFACE_H

#include "PSXStruct.h"
#include <Arduino.h>

// PSX Controller button response mask (Analogue red variant)
#define SELECT    0x01
#define JOYRIGHT  0x02
#define JOYLEFT   0x04
#define START     0x08
#define UP        0x10
#define RIGHT     0x20
#define DOWN      0x40
#define LEFT      0x80
#define L2        0x01
#define R2        0x02
#define L1        0x04
#define R1        0x08
#define TRIANGLE  0x10
#define CIRCLE    0x20
#define CROSS     0x40
#define SQUARE    0x80


/**
 * Class for interfacing with a PSX DualShock Controller mainboard.
 */

class PSXController {
    public:
        PSXController(byte data, byte cmnd, byte att, byte clk, double deadzone = 0.0);
        struct PSXState* poll();

    private:
        void debug(byte* data);
        byte reverseByte(byte data);
        struct PSXState* respToState(byte* data);
        byte addDeadzone(byte analogueData);
        
        byte dataPin;
        byte commandPin;;
        byte attentionPin;
        byte clockPin;
        double deadzonePercent;
};

#endif // PSX_INTERFACE_H
