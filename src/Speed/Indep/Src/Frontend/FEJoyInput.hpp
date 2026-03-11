#ifndef FRONTEND_FEJOYINPUT_H
#define FRONTEND_FEJOYINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct ActionQueue;

enum JoystickPort {
    kJP_Port0 = 0,
    kJP_Port1 = 1,
    kJP_Port2 = 2,
    kJP_Port3 = 3,
    kJP_NumPorts = 4
};

// total size: 0x8
struct cFEngJoyInput {
    ActionQueue* mActionQ[2]; // offset 0x0

    static cFEngJoyInput* mInstance;

    static cFEngJoyInput* Get() { return mInstance; }
    cFEngJoyInput();
    void FlushActions();
    void JoyDisable(JoystickPort port, bool do_flush);
    bool IsJoyPluggedIn(JoystickPort port);
    void JoyEnable(JoystickPort port, bool do_flush);
    bool IsJoyEnabled(JoystickPort port);
    void SetRequiredJoy(JoystickPort port, bool required);
    bool CheckUnplugged();
    void HandleJoy();
    unsigned long GetJoyPadMask(unsigned char pPadIndex);
};

#endif
