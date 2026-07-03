#ifndef FRONTEND_MENUSCREENS_CONTROLLERUNPLUGGED_H
#define FRONTEND_MENUSCREENS_CONTROLLERUNPLUGGED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

// total size: 0x30
class ControllerUnplugged : public MenuScreen {
  public:
    ControllerUnplugged(ScreenConstructorData *sd);
    ~ControllerUnplugged() override;
    void NotificationMessage(u32 msg, struct FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup();

    JoystickPort port; // offset 0x2C, size 0x4
};
#endif
