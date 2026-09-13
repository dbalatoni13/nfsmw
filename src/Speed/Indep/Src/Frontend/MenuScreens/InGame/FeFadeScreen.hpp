#ifndef FEFADESCREEN_HPP
#define FEFADESCREEN_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class FadeScreen : public MenuScreen {
  public:
    FadeScreen(ScreenConstructorData *sd);
    ~FadeScreen() override;
    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override;
    static bool IsFadeScreenOn();
};

#endif
