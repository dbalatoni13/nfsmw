#ifndef UIQRCARSELECT_H
#define UIQRCARSELECT_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

struct FEImage;
struct FEString;
struct FECarRecord;
struct FECareerRecord;
struct SelectableCar;

// File: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp
// total size: 0x1C
// Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:15
class QRCarSelectBustedManager {
  public:
    typedef enum { BUSTED_ANIM_NOTHING = 0, BUSTED_ANIM_SHOW_STRIKE = 1, BUSTED_ANIM_SHOW_IMPOUNDED = 2 } eBustedAnimationTypes;

    FECareerRecord *WorkingCareerRecord; // offset 0x0, size 0x4
    FECarRecord *WorkingCarRecord;       // offset 0x4, size 0x4
    eBustedAnimationTypes Flags;         // offset 0x8, size 0x4

    QRCarSelectBustedManager(const char *pkg_name, int flags); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:25
    virtual ~QRCarSelectBustedManager();                       // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:26

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2);

    static void TextureLoadedCallbackAccessor(uint32 this_screen) {
        reinterpret_cast<QRCarSelectBustedManager *>(this_screen)->TextureLoadedCallback();
    }

    void TextureLoadedCallback();

    void LoadImpoundTexture();

    void MaybeReleaseCar(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:33

    void MaybeAddImpoundBox(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:34

    void SetSelectedCar(FECarRecord *record);

    static void SetPlayerBusted() {
        bPlayerJustGotBusted = true;
    }

    static void SetIsCross() {}

    bool IsImpoundInfoVisible();

    bool ShowImpoundedTexture();
    bool ShowNewStrikeAnimation() {}
    bool ShowImpoundedAnimation() {}

  private:
    const char *GetPackageName() {}
    void RefreshHeader(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:54
    void PayInfractions();
    void UseInfractionMarker();
    void UseImpoundMarker();
    bool CalcGameOver(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:60
    void CalcInfractionStatus();
    void BustCar();
    void ImpoundCar();

    static bool bPlayerJustGotBusted; // size: 0x1, address: 0x80439150, Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.cpp:24
    static bool bIsCross;             // size: 0x1, address: 0xFFFFFFFF

    uint32 ImpoundStampHash; // offset 0xC, size 0x4
    char *ParentPkg;         // offset 0x10, size 0x4
    bool bWantsImpound;      // offset 0x14, size 0x1
};

// total size: 0x1B8
class UIQRCarSelect : public MenuScreen {
  public:
    enum CarLists {
        LIST_STOCK = 0,
        LIST_CAREER = 1,
        LIST_QUICK_RACE = 2,
        LIST_BONUS = 3,
        LIST_PRESET = 4,
        LIST_DEBUG = 5,
        NUM_LISTS = 6,
    };

    UIQRCarSelect(ScreenConstructorData *sd);
    ~UIQRCarSelect() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void Setup();
    void SetupForPlayer(int player);
    void InitStatsSliders();
    void RefreshCarList();
    void ClearCarList();
    void ScrollCars(eScrollDir dir);
    void ScrollLists(eScrollDir dir);
    void RefreshHeader();
    void UpdateSliders();
    void ChooseTransmission();
    void SetSelectedCar(SelectableCar *newCar, int player_num);
    FECarRecord *GetSelectedCarRecord();
    bool IsCarImpounded(uint32 handle);
    int GetFilterType();

    void CommitChangeStartRace(bool allowError);
    int GetBonusUnlockText(FECarRecord *fe_car);
    int GetBonusUnlockBinNumber(FECarRecord *fe_car);
    void RefreshBonusCarList();
    void OnlineActOnSelect();

    static uint32 ForceCar; // size: 0x4

    bTList<SelectableCar> FilteredCarsList;    // offset 0x2C, size 0x8
    SelectableCar *pSelectedCar;               // offset 0x34, size 0x4
    Timer tLastEventTimer;                     // offset 0x38, size 0x4
    FEImage *pManuLogo;                        // offset 0x3C, size 0x4
    FEImage *pCarBadge;                        // offset 0x40, size 0x4
    FEString *pCarName;                        // offset 0x44, size 0x4
    FEString *pCarNameShadow;                  // offset 0x48, size 0x4
    FEString *pFilter;                         // offset 0x4C, size 0x4
    uint32 ListHandles[6];                     // offset 0x50, size 0x18
    uint32 originalCar;                        // offset 0x68, size 0x4
    QRCarSelectBustedManager TheBustedManager; // offset 0x6C, size 0x1C
    CustomizeMeter TheHeatMeter;               // offset 0x88, size 0x50
    TwoStageSlider AccelerationSlider;         // offset 0xD8, size 0x44
    TwoStageSlider TopSpeedSlider;             // offset 0x11C, size 0x44
    TwoStageSlider HandlingSlider;             // offset 0x160, size 0x44
    bool bLoadingBarActive;                    // offset 0x1A4, size 0x1
    bool bShowcaseMode;                        // offset 0x1A8, size 0x1
    int iPlayerNum;                            // offset 0x1AC, size 0x4
    int filter;                                // offset 0x1B0, size 0x4
    uint32 iPrevButtonMsg;                     // offset 0x1B4, size 0x4
};

#endif
