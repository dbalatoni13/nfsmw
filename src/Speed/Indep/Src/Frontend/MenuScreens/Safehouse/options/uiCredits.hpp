#ifndef __UICREDITS_HPP__
#define __UICREDITS_HPP__

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/UnicodeFile.hpp"

// total size: 0x48
class uiCredits : public MenuScreen {
  public:
    uiCredits(ScreenConstructorData *sd);
    ~uiCredits() override {}
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    bool initComplete_;       // offset 0x2C, size 0x1
    FEString *prototypeStr_;  // offset 0x30, size 0x4
    i16 *creditLine_;         // offset 0x34, size 0x4
    FEObject *pendingDelete_; // offset 0x38, size 0x4
    UnicodeFile uf_;          // offset 0x3C, size 0xC
};

#endif
