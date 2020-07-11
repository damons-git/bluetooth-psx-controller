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

/**
 * Class for interfacing with a PSX DualShock Controller mainboard.
 */

class PSXController {
    public:
        PSXController(byte data, byte cmnd, byte att, byte clk);
        struct PSXState* poll();

    private:
        byte dataPin;
        byte commandPin;;
        byte attentionPin;
        byte clockPin;
};

#endif // PSX_INTERFACE_H
