#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIINFRACTIONS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIINFRACTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class PostPursuitInfractionsScreen : MenuScreen {
  private:
    void NotifyBustedTextureLoaded();
    uint32 CalcBustedTexture();

  public:
    PostPursuitInfractionsScreen(ScreenConstructorData *sd);
    ~PostPursuitInfractionsScreen() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    static MenuScreen *Create(ScreenConstructorData *sd) {
        return new ("", 0) PostPursuitInfractionsScreen(sd);
    };
    static void TextureLoadedCallback(uint32 arg) {
        reinterpret_cast<PostPursuitInfractionsScreen *>(arg)->NotifyBustedTextureLoaded();
    }

  private:
    FECareerRecord *WorkingCareerRecord; // offset 0x2C
    bool bStrikeLimitReached;            // offset 0x30
    int AmountToPay;                     // offset 0x34
    int AmountPlayerHas;                 // offset 0x38
    bool bHasMarker;                     // offset 0x3C
    uint32 BustedTexture;                // offset 0x40
    bool bFirstTimeBusted;               // offset 0x44
};

#endif
