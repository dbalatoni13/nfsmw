#ifndef UIQRPRESSSTART_H
#define UIQRPRESSSTART_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

#include <types.h>

// total size: 0x34
class uiQRPressStart : public MenuScreen {
  public:
    uiQRPressStart(ScreenConstructorData *sd);
    ~uiQRPressStart() override;

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup();

    int iPlayerNum; // offset 0x2C, size 0x4
    int param;      // offset 0x30, size 0x4
};

#endif
