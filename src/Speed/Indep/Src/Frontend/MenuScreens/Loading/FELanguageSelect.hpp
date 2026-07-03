#ifndef LANGUAGESELECTSCREEN_HPP
#define LANGUAGESELECTSCREEN_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FELanguageSelect.hpp
// total size: 0x170
// Decl: speed/indep/src/frontend/menuscreens/loading/FELanguageSelect.hpp:14
class LanguageSelectScreen : public IconScrollerMenu {
  public:
    LanguageSelectScreen(ScreenConstructorData *sd);
    ~LanguageSelectScreen() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    void Setup() override {}

  private:
    static bool bChoiceMade; // size: 0x1, address: 0xFFFFFFFF

    Timer StartedTimer; // offset 0x16C
};

#endif
