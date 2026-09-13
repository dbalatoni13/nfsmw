#ifndef UISIXDAYSLATER_Hpp
#define UISIXDAYSLATER_Hpp

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"

class SixDaysLater : public MenuScreen {
  public:
    SixDaysLater(ScreenConstructorData *sd);
    ~SixDaysLater() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    FEString *mpDataMainString; // offset 0x2C
    int mStringMode;            // offset 0x30
};

#endif
