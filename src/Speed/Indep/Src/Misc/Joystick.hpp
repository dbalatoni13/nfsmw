#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

// File: speed/indep/src/misc/Joystick.hpp
// Decl: speed/indep/src/misc/Joystick.hpp:18
typedef enum {
    JOYSTICK_PORT_NONE = -1,
    JOYSTICK_PORT1 = 0,
    JOYSTICK_PORT2 = 1,
    JOYSTICK_PORT3 = 2,
    JOYSTICK_PORT4 = 3,
    JOYSTICK_PORT_ALL = 4
} JoystickPort;

#define NUM_JOYSTICK_PORTS 2  // :37
#define DEBUG_JOYSTICK_PORT 1 // :43

#endif
