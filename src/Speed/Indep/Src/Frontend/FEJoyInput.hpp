#ifndef FEJOYINPUT_H
#define FEJOYINPUT_H

#include <types.h>
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/Joystick.hpp"

// total size: 0x8
// Decl: 21
class cFEngJoyInput {
  public:
    static cFEngJoyInput *mInstance;     // size: 0x4, address: 0x8041B810, Decl: 23
    static struct cFEngJoyInput *Get() { // Decl: 24
        return mInstance;
    }

    cFEngJoyInput();  // Decl: 30
    ~cFEngJoyInput(); // Decl: 31

    void SetRequiredJoy(JoystickPort port, bool required); // Decl: 33
    bool IsRequiredJoy(JoystickPort port);                 // Decl: 34
    bool IsJoyPluggedIn(JoystickPort port);                // Decl: 35
    void JoyDisable(JoystickPort port, bool do_flush);     // Decl: 36
    void JoyEnable(JoystickPort port, bool do_flush);      // Decl: 37
    bool IsJoyEnabled(JoystickPort port);                  // Decl: 38

    void FlushActions(); // Decl: 41
    void ClearInputQueue();
    void HandleJoy(); // Decl: 43

    u32 GetJoyPadMask(u8 pPadIndex); // Decl: 45

    uint32 GetJoyPadTexture(const char *eventString, JoystickPort port);

  private:
    bool CheckUnplugged(); // Decl: 72

    ActionQueue *mActionQ[2]; // offset 0x0, size 0x8, Decl: 76
};

#endif
