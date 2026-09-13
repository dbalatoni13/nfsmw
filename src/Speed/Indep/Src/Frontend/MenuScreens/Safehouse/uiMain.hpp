#ifndef _UIMAIN
#define _UIMAIN

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// total size: 0x170
class UIMain : public IconScrollerMenu {
  public:
    UIMain(ScreenConstructorData *sd);
    ~UIMain() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
    void UpdateProfileData();

    bool m_bStatsShowing; // offset 0x16C, size 0x1
};

#endif
