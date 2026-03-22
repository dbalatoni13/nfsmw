#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

struct FEImage;
enum eScrollDir;

// total size: 0x1C0
struct uiRepSheetMain : public IconScrollerMenu {
    bool bIsInGame;                        // offset 0x16C, size 0x1
    bool bBossAvailable;                   // offset 0x170, size 0x1
    bool bBossBeaten;                      // offset 0x174, size 0x1
    FEImage* pRivalImg;                    // offset 0x178, size 0x4
    FEImage* pTagImg;                      // offset 0x17C, size 0x4
    unsigned int DefeatedTextureHash;      // offset 0x180, size 0x4
    uiRepSheetRivalStreamer RivalStreamer;  // offset 0x184, size 0x3C

    static void TextureLoadedCallback(unsigned int arg);

    uiRepSheetMain(ScreenConstructorData* sd);
    ~uiRepSheetMain() override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;

    void NotifyTextureLoaded();
    unsigned int GetDefeatedTexture();
    void UpdateInfo();
    void ScrollRival(eScrollDir dir);
};

#endif
