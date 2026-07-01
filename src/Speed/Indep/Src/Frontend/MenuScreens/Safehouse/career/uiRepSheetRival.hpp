#ifndef __UI_REP_SHEET_RIVAL_HPP__
#define __UI_REP_SHEET_RIVAL_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

// total size: 0x8C
class uiRepSheetRival : public MenuScreen {
  public:
    typedef enum {
        UIREPSHEETRIVAL_FE = 0,
        UIREPSHEETRIVAL_INGAME = 1,
        UIREPSHEETRIVAL_INGAME_MIDFLOW = 2,
        UIREPSHEETRIVAL_INGAME_ONEOFF = 3
    } UIREPSHEETRIVAL;

    uiRepSheetRival(ScreenConstructorData *sd);
    ~uiRepSheetRival() override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    void RefreshHeader();

    static void TextureLoadedCallback(uint32 arg) {
        reinterpret_cast<uiRepSheetRival *>(arg)->NotifyTextureLoaded();
    };
    void NotifyTextureLoaded();
    uint32 GetDefeatedTexture();

  private:
    void Setup();
    void SetupRace(uint32 num, GRaceParameters *race);

    bool bIsInGame;                        // offset 0x2C, size 0x1
    bool bMidRivalFlow;                    // offset 0x30, size 0x1
    bool bOneOff;                          // offset 0x34, size 0x1
    GRaceParameters *launch_race;          // offset 0x38, size 0x4
    FEImage *pRivalImg;                    // offset 0x3C, size 0x4
    FEImage *pDefeatedImg;                 // offset 0x40, size 0x4
    FEImage *pDefeatedImgBG;               // offset 0x44, size 0x4
    FEImage *pTagImg;                      // offset 0x48, size 0x4
    FEImage *pBGImg;                       // offset 0x4C, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x50, size 0x3C
};

#endif
