#ifndef FEJOYINPUT_H
#define FEJOYINPUT_H

#include <types.h>
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/Joystick.hpp"

// total size: 0x8
// Decl: speed/indep/src/frontend/FEJoyInput.hpp:21
class cFEngJoyInput {
  public:
    static cFEngJoyInput *mInstance;      // size: 0x4, address: 0x8041B810, Decl: speed/indep/src/frontend/FEJoyInput.hpp:23
    static struct cFEngJoyInput *Get() {} // Decl: speed/indep/src/frontend/FEJoyInput.hpp:24

    cFEngJoyInput();  // Decl: speed/indep/src/frontend/FEJoyInput.hpp:30
    ~cFEngJoyInput(); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:31

    void SetRequiredJoy(JoystickPort port, bool required); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:33
    bool IsRequiredJoy(JoystickPort port);                 // Decl: speed/indep/src/frontend/FEJoyInput.hpp:34
    bool IsJoyPluggedIn(JoystickPort port);                // Decl: speed/indep/src/frontend/FEJoyInput.hpp:35
    void JoyDisable(JoystickPort port, bool do_flush);     // Decl: speed/indep/src/frontend/FEJoyInput.hpp:36
    void JoyEnable(JoystickPort port, bool do_flush);      // Decl: speed/indep/src/frontend/FEJoyInput.hpp:37
    bool IsJoyEnabled(JoystickPort port);                  // Decl: speed/indep/src/frontend/FEJoyInput.hpp:38

    void FlushActions(); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:41
    void ClearInputQueue();
    void HandleJoy(); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:43

    u32 GetJoyPadMask(u8 pPadIndex); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:45

    uint32 GetJoyPadTexture(const char *eventString, JoystickPort port);

  private:
    bool CheckUnplugged(); // Decl: speed/indep/src/frontend/FEJoyInput.hpp:72

    ActionQueue *mActionQ[2]; // offset 0x0, size 0x8, Decl: speed/indep/src/frontend/FEJoyInput.hpp:76
};

#endif
