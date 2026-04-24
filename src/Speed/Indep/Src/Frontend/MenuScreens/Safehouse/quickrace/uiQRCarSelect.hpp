#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCARSELECT_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCARSELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
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

// total size: 0x1C
class QRCarSelectBustedManager {
  public:
    enum eBustedAnimationTypes {
        BUSTED_ANIM_NOTHING = 0,
        BUSTED_ANIM_SHOW_STRIKE = 1,
        BUSTED_ANIM_SHOW_IMPOUNDED = 2,
    };

    static void TextureLoadedCallbackAccessor(unsigned int this_screen) {
        reinterpret_cast<QRCarSelectBustedManager *>(this_screen)->TextureLoadedCallback();
    }

    static void SetPlayerBusted() {
        bPlayerJustGotBusted = true;
    }

    static void SetIsCross() {}

    bool ShowNewStrikeAnimation() {}

    bool ShowImpoundedAnimation() {}

    QRCarSelectBustedManager(const char *pkg_name, int flags);

    virtual ~QRCarSelectBustedManager();

    bool IsImpoundInfoVisible();

    bool ShowImpoundedTexture();

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2);

    void TextureLoadedCallback();

    void LoadImpoundTexture();

    void SetSelectedCar(FECarRecord *record);

    void MaybeReleaseCar();

    void MaybeAddImpoundBox();

    FECareerRecord *WorkingCareerRecord; // offset 0x0, size 0x4
    FECarRecord *WorkingCarRecord;       // offset 0x4, size 0x4
    enum eBustedAnimationTypes Flags;    // offset 0x8, size 0x4

  private:
    const char *GetPackageName() {}

    void RefreshHeader();
    bool CalcGameOver();

    static bool bPlayerJustGotBusted; // size: 0x1, address: 0x80439150
    static bool bIsCross;             // size: 0x1, address: 0xFFFFFFFF

    unsigned int ImpoundStampHash; // offset 0xC, size 0x4
    const char *ParentPkg;         // offset 0x10, size 0x4
    bool bWantsImpound;            // offset 0x14, size 0x1
};

// total size: 0x1B8
struct UIQRCarSelect : public MenuScreen {
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

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;

    bool IsCarImpounded(unsigned int handle);
    void CommitChangeStartRace(bool allowError);
    void Setup();
    void InitStatsSliders();
    void UpdateSliders();
    int GetFilterType();
    void SetupForPlayer(int player);
    int GetBonusUnlockText(FECarRecord *fe_car);
    int GetBonusUnlockBinNumber(FECarRecord *fe_car);
    void RefreshHeader();
    void ChooseTransmission();
    FECarRecord *GetSelectedCarRecord();
    void SetSelectedCar(SelectableCar *newCar, int player_num);
    void RefreshBonusCarList();
    void RefreshCarList();
    void ClearCarList();
    void ScrollCars(eScrollDir dir);
    void ScrollLists(eScrollDir dir);
    void OnlineActOnSelect();

    static unsigned int ForceCar; // size: 0x4

    bTList<SelectableCar> FilteredCarsList;    // offset 0x2C, size 0x8
    SelectableCar *pSelectedCar;               // offset 0x34, size 0x4
    Timer tLastEventTimer;                     // offset 0x38, size 0x4
    FEImage *pManuLogo;                        // offset 0x3C, size 0x4
    FEImage *pCarBadge;                        // offset 0x40, size 0x4
    FEString *pCarName;                        // offset 0x44, size 0x4
    FEString *pCarNameShadow;                  // offset 0x48, size 0x4
    FEString *pFilter;                         // offset 0x4C, size 0x4
    unsigned int ListHandles[6];               // offset 0x50, size 0x18
    unsigned int originalCar;                  // offset 0x68, size 0x4
    QRCarSelectBustedManager TheBustedManager; // offset 0x6C, size 0x1C
    CustomizeMeter TheHeatMeter;               // offset 0x88, size 0x50
    TwoStageSlider AccelerationSlider;         // offset 0xD8, size 0x44
    TwoStageSlider TopSpeedSlider;             // offset 0x11C, size 0x44
    TwoStageSlider HandlingSlider;             // offset 0x160, size 0x44
    bool bLoadingBarActive;                    // offset 0x1A4, size 0x1
    bool bShowcaseMode;                        // offset 0x1A8, size 0x1
    int iPlayerNum;                            // offset 0x1AC, size 0x4
    int filter;                                // offset 0x1B0, size 0x4
    unsigned int iPrevButtonMsg;               // offset 0x1B4, size 0x4
};

#endif
