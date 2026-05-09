#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

// total size: 0x60
class RepSheetIcon : public IconOption {
  public:
    RepSheetIcon(uint32 tex_hash, uint32 name_hash, uint32 the_id) : IconOption(tex_hash, name_hash, 0) {
        id = the_id;
    }
    ~RepSheetIcon() override {}
    void React(const char *pkg_name, uint32 data, struct FEObject *obj, uint32 param1, uint32 param2) override;

    uint32 id; // offset 0x5C, size 0x4
};

// total size: 0x1C0
class uiRepSheetMain : public IconScrollerMenu {
  public:
    static void TextureLoadedCallback(uint32 arg);

    uiRepSheetMain(ScreenConstructorData *sd);
    ~uiRepSheetMain() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

    void NotifyTextureLoaded();
    uint32 GetDefeatedTexture();
    void UpdateInfo();

  private:
    void Setup() override;
    void ScrollRival(eScrollDir dir);

    bool bIsInGame;                        // offset 0x16C, size 0x1
    bool bBossAvailable;                   // offset 0x170, size 0x1
    bool bBossBeaten;                      // offset 0x174, size 0x1
    FEImage *pRivalImg;                    // offset 0x178, size 0x4
    FEImage *pTagImg;                      // offset 0x17C, size 0x4
    uint32 DefeatedTextureHash;            // offset 0x180, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x184, size 0x3C
};

#endif
