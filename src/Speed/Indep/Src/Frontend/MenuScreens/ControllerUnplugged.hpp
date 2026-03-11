#ifndef FRONTEND_MENUSCREENS_CONTROLLERUNPLUGGED_H
#define FRONTEND_MENUSCREENS_CONTROLLERUNPLUGGED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

// total size: 0x30
struct ControllerUnplugged : public MenuScreen {
    JoystickPort port; // offset 0x2C

    ControllerUnplugged(ScreenConstructorData* sd);
    ~ControllerUnplugged() override;
    static MenuScreen* Create(ScreenConstructorData* sd);
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    void Setup();
};

#endif
