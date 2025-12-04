#ifndef FRONTEND_HUD_FEPKG_HUD_H
#define FRONTEND_HUD_FEPKG_HUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

enum ePlayerHudType {
    PHT_NONE = 0,
    PHT_STANDARD = 1,
    PHT_DRAG = 2,
    PHT_SPLIT1 = 3,
    PHT_SPLIT2 = 4,
    PHT_DRAG_SPLIT1 = 5,
    PHT_DRAG_SPLIT2 = 6,
};

// total size: 0x348
class FEngHud : public UTL::COM::Object, public IHud {
  public:
    FEngHud(ePlayerHudType ht, const char *pkg_name, IPlayer *player, int player_number);

  private:
    unsigned long long CurrentHudFeatures; // offset 0x20, size 0x8
    ePlayerHudType mPlayerHudType;         // offset 0x28, size 0x4
    const char *pPackageName;              // offset 0x2C, size 0x4
    IPlayer *pPlayer;                      // offset 0x30, size 0x4
    int PlayerNumber;                      // offset 0x34, size 0x4
    ActionQueue mActionQ;                  // offset 0x38, size 0x294
    bool mInPursuit;                       // offset 0x2CC, size 0x1
    bool mHasTurbo;                        // offset 0x2D0, size 0x1
    HudElement *pSpeedometer;              // offset 0x2D4, size 0x4
    HudElement *pTachometer;               // offset 0x2D8, size 0x4
    HudElement *pTachometerDrag;           // offset 0x2DC, size 0x4
    HudElement *pShiftUpdater;             // offset 0x2E0, size 0x4
    HudElement *pCostToState;              // offset 0x2E4, size 0x4
    HudElement *pReputation;               // offset 0x2E8, size 0x4
    HudElement *pHeatMeter;                // offset 0x2EC, size 0x4
    HudElement *pTurboMeter;               // offset 0x2F0, size 0x4
    HudElement *pEngineTemp;               // offset 0x2F4, size 0x4
    HudElement *pNitrous;                  // offset 0x2F8, size 0x4
    HudElement *pSpeedBreakerMeter;        // offset 0x2FC, size 0x4
    HudElement *pRaceOverMessage;          // offset 0x300, size 0x4
    HudElement *pGenericMessage;           // offset 0x304, size 0x4
    HudElement *pAutoSaveIcon;             // offset 0x308, size 0x4
    HudElement *pRaceInformation;          // offset 0x30C, size 0x4
    HudElement *pLeaderBoard;              // offset 0x310, size 0x4
    HudElement *pPursuitBoard;             // offset 0x314, size 0x4
    HudElement *pMilestoneBoard;           // offset 0x318, size 0x4
    HudElement *pBustedMeter;              // offset 0x31C, size 0x4
    HudElement *pTimeExtension;            // offset 0x320, size 0x4
    HudElement *pWrongWIndi;               // offset 0x324, size 0x4
    HudElement *pOnlineSupport;            // offset 0x328, size 0x4
    HudElement *p321Go;                    // offset 0x32C, size 0x4
    HudElement *pRadarDetector;            // offset 0x330, size 0x4
    HudElement *pMinimap;                  // offset 0x334, size 0x4
    HudElement *pGetAwayMeter;             // offset 0x338, size 0x4
    HudElement *pMenuZoneTrigger;          // offset 0x33C, size 0x4
    HudElement *pInfractions;              // offset 0x340, size 0x4
    bool mCurrentWidescreenSetting;        // offset 0x344, size 0x1
};

// total size: 0xC
class HudResourceManager {
  public:
    enum HudResourceLoadStates {
        HRM_NOT_LOADED = 0,
        HRM_LOADING_IN_PROGRESS = 1,
        HRM_LOADED = 2,
        HRM_UNLOADING_IN_PROGRESS = 3,
    };

    static const char *GetHudFengName(ePlayerHudType ht);

  private:
    HudResourceLoadStates mHudResourcesState; // offset 0x0, size 0x4
    ResourceFile *pHudTextures;               // offset 0x4, size 0x4
};

#endif
