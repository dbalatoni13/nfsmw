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

    static cFEngJoyInput* Get();
    cFEngJoyInput();
    void FlushActions();
    void JoyDisable();
    bool IsJoyPluggedIn(JoystickPort port);
    void JoyEnable();
    bool IsJoyEnabled();
    void SetRequiredJoy(JoystickPort port);
    void CheckUnplugged();
    void HandleJoy();
    unsigned long GetJoyPadMask(unsigned char port);
};

#endif
