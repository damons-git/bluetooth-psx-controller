#ifndef PSX_STRUCT_H
#define PSX_STRUCT_H

#include <Arduino.h>

/**
 * PSX controller state.
 * A struct containing mappings for all buttons on a standard PlayStation gamepad.
 */

struct PSXState {
    public:
        bool cross;           // Cross button
        bool circle;          // Circle button
        bool square;          // Square button
        bool triangle;        // Triangle button
        bool lb;              // Left back (upper) bumper button
        bool rb;              // Right back (upper) bumper button
        bool lt;              // Left trigger
        bool rt;              // Right trigger
        bool select;          // Select button
        bool start;           // Start button
        bool lthumb;          // Left-thumb stick pressed in button
        bool rthumb;          // Right-thumb stick pressed in button
        bool up;              // D-pad up
        bool down;            // D-pad down
        bool left;            // D-pad left
        bool right;           // D-pad right
        short int lthumb_x;   // X-coordinate of left thumb stick
        short int lthumb_y;   // Y-coordinate of left thumb stick
        short int rthumb_x;   // X-coordinate of right thumb stick
        short int rthumb_y;   // Y-coordinate of right thumb stick
        PSXState();
};

#endif PSX_STRUCT_H
