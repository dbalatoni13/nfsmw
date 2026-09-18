#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIINFRACTIONS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIINFRACTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class FECareerRecord;

// total size: 0x48
class PostPursuitInfractionsScreen : public MenuScreen {
  public:
    static MenuScreen *Create(ScreenConstructorData *sd);
    static void TextureLoadedCallback(uint32 arg);

    PostPursuitInfractionsScreen(ScreenConstructorData *sd);
    ~PostPursuitInfractionsScreen() override;

    void NotifyBustedTextureLoaded();
    uint32 CalcBustedTexture();
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    FECareerRecord *WorkingCareerRecord; // offset 0x2C, size 0x4
    bool bStrikeLimitReached;            // offset 0x30, size 0x1
    int AmountToPay;                     // offset 0x34, size 0x4
    int AmountPlayerHas;                 // offset 0x38, size 0x4
    bool bHasMarker;                     // offset 0x3C, size 0x1
    uint32 BustedTexture;                // offset 0x40, size 0x4
    bool bFirstTimeBusted;               // offset 0x44, size 0x1
};

#endif
