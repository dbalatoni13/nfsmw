#ifndef FRONTEND_FEJOYINPUT_H
#define FRONTEND_FEJOYINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/Input/ActionQueue.h"

typedef enum {
    JOYSTICK_PORT_NONE = -1,
    JOYSTICK_PORT1 = 0,
    JOYSTICK_PORT2 = 1,
    JOYSTICK_PORT3 = 2,
    JOYSTICK_PORT4 = 3,
    JOYSTICK_PORT_ALL = 4
} JoystickPort;

// total size: 0x8
class cFEngJoyInput {
  public:
    static inline struct cFEngJoyInput *Get() {}

    cFEngJoyInput();
    ~cFEngJoyInput();
    void SetRequiredJoy(JoystickPort port, bool required);
    bool IsRequiredJoy(JoystickPort port);
    bool IsJoyPluggedIn(JoystickPort port);
    void JoyDisable(JoystickPort port, bool do_flush);
    void JoyEnable(JoystickPort port, bool do_flush);
    bool IsJoyEnabled(JoystickPort port);
    void FlushActions();
    void ClearInputQueue();
    void HandleJoy();
    u32 GetJoyPadMask(u8 pPadIndex);
    uint32 GetJoyPadTexture(const char *eventString, JoystickPort port);

    static cFEngJoyInput *mInstance; // size: 0x4, address: 0x8041B810

  private:
    bool CheckUnplugged();

    ActionQueue *mActionQ[2]; // offset 0x0, size 0x8
};

#endif
