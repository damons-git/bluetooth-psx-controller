#include "PSXStruct.h"
    
PSXState::PSXState() {
    cross     = false;
    circle    = false;
    square    = false;
    triangle  = false;
    lb        = false;
    rb        = false;
    lt        = false;
    rt        = false;
    select    = false;
    start     = false;
    lthumb    = false;
    rthumb    = false;
    up        = false;
    down      = false;
    left      = false;
    right     = false;
    lthumb_x  = 0.0;
    lthumb_y  = 0.0;
    rthumb_x  = 0.0;
    rthumb_y  = 0.0;
};
