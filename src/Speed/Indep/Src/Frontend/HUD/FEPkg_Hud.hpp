#ifndef FEPKG_HUD_HPP
#define FEPKG_HUD_HPP

#include "Speed/Indep/Src/Frontend/HUD/FeHudResourceManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Frontend/HUD/FeOnlineHudSupport.hpp"

// File: speed/indep/src/frontend/hud/FEPkg_Hud.hpp
// total size: 0x348
// Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:14
class FEngHud : public UTL::COM::Object, public IHud {
  public:
    FEngHud(ePlayerHudType ht, const char *pkg_name, IPlayer *player, int player_number);
    ~FEngHud() override;

    void Release() override { // Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:20
        delete this;
    }
    void Update(IPlayer *player, float dT) override;

    void JoyDisable() override;
    void JoyEnable() override;
    void JoyHandle(IPlayer *player);

    bool AreResourcesLoaded() override;
    bool IsHudVisible() override { // Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:27
        return CurrentHudFeatures != 0;
    }

    OnlineHUDSupport *GetOnlineHUDSupport() {
        return static_cast<OnlineHUDSupport *>(pOnlineSupport);
    }

    void HideAll() override { // Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:40
        SetHudFeatures(0);
    }

    void FadeAll(bool fadeIn) override;

    void SetInPursuit(bool inPursuit) override;

    bool IsInPursuit();

    void SetHasTurbo(bool hasTurbo) override {
        mHasTurbo = hasTurbo;
    }

    bool DoesHaveTurbo() {}

    bool IsSplitScreen() {}

    void RefreshMiniMapItems() override;

    static bool ShouldRearViewMirrorBeVisible(EVIEW_ID viewId);

    static float ChooseMaxRpmTextureNumber(float maxRpm);

    static bool bIsRestartingRace; // size: 0x1, address: 0x8041BE48

  private:
    void SetHudFeatures(uint64 hud_features);
    uint64 DetermineHudFeatures(IPlayer *player);
    void SetWideScreenMode();

    HudFeaturesType CurrentHudFeatures; // offset 0x20, size 0x8, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:57
    ePlayerHudType mPlayerHudType;      // offset 0x28, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:58
    const char *pPackageName;           // offset 0x2C, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:59
    IPlayer *pPlayer;                   // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:60
    int PlayerNumber;                   // offset 0x34, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:61
    ActionQueue mActionQ;               // offset 0x38, size 0x294, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:62

    bool mInPursuit; // offset 0x2CC, size 0x1
    bool mHasTurbo;  // offset 0x2D0, size 0x1

    HudElement *pSpeedometer;       // offset 0x2D4, size 0x4
    HudElement *pTachometer;        // offset 0x2D8, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:76
    HudElement *pTachometerDrag;    // offset 0x2DC, size 0x4
    HudElement *pShiftUpdater;      // offset 0x2E0, size 0x4
    HudElement *pCostToState;       // offset 0x2E4, size 0x4
    HudElement *pReputation;        // offset 0x2E8, size 0x4
    HudElement *pHeatMeter;         // offset 0x2EC, size 0x4
    HudElement *pTurboMeter;        // offset 0x2F0, size 0x4
    HudElement *pEngineTemp;        // offset 0x2F4, size 0x4
    HudElement *pNitrous;           // offset 0x2F8, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:79
    HudElement *pSpeedBreakerMeter; // offset 0x2FC, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:80
    HudElement *pRadarDetector;     // offset 0x330, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:81
    HudElement *pRaceOverMessage;   // offset 0x300, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:84
    HudElement *pGenericMessage;    // offset 0x304, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:85
    HudElement *pAutoSaveIcon;      // offset 0x308, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:86
    HudElement *pRaceInformation;   // offset 0x30C, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:87
    HudElement *pLeaderBoard;       // offset 0x310, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:90
    HudElement *pPursuitBoard;      // offset 0x314, size 0x4
    HudElement *pMilestoneBoard;    // offset 0x318, size 0x4
    HudElement *pBustedMeter;       // offset 0x31C, size 0x4
    HudElement *pTimeExtension;     // offset 0x320, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:92
    HudElement *pWrongWIndi;        // offset 0x324, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:93
    HudElement *pOnlineSupport;     // offset 0x328, size 0x4
    HudElement *p321Go;             // offset 0x32C, size 0x4
    HudElement *pMinimap;           // offset 0x334, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:94
    HudElement *pGetAwayMeter;      // offset 0x338, size 0x4
    HudElement *pMenuZoneTrigger;   // offset 0x33C, size 0x4, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:95
    HudElement *pInfractions;       // offset 0x340, size 0x4
    bool mCurrentWidescreenSetting; // offset 0x344, size 0x1, Decl: speed/indep/src/frontend/hud/FEPkg_Hud.hpp:105
};

extern HudResourceManager TheHudResourceManager;

inline void HudResourceManager::LoadingCompleteCallbackBridge(int32 param) {
    reinterpret_cast<HudResourceManager *>(param)->LoadingCompleteCallback();
}
inline void HudResourceManager::LoadingCompleteCallbackBridge(uint32 param) {
    reinterpret_cast<HudResourceManager *>(param)->LoadingCompleteCallback();
}
inline void HudResourceManager::LoadedCustomHudTexturePackCallbackBridge(uint32 param) {
    reinterpret_cast<HudResourceManager *>(param)->LoadedCustomHudTexturePackCallback();
}
inline void HudResourceManager::LoadedCustomHudTexturesCallbackBridge(uint32 param) {
    reinterpret_cast<HudResourceManager *>(param)->LoadedCustomHudTexturesCallback();
}

void HideEverySingleHud();

#endif
