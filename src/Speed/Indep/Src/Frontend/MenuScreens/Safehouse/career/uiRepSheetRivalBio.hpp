#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALBIO_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALBIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

struct FEImage;

// total size: 0x78
struct uiRepSheetRivalBio : public MenuScreen {
    bool bIsInGame;                        // offset 0x2C, size 0x1
    FEImage* pRivalImg;                    // offset 0x30, size 0x4
    FEImage* pTagImg;                      // offset 0x34, size 0x4
    FEImage* pBGImg;                       // offset 0x38, size 0x4
    uiRepSheetRivalStreamer RivalStreamer;  // offset 0x3C, size 0x3C

    uiRepSheetRivalBio(ScreenConstructorData* sd);
    ~uiRepSheetRivalBio() override {}

    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) override;

    void RefreshHeader();
    void Setup();
};

#endif
