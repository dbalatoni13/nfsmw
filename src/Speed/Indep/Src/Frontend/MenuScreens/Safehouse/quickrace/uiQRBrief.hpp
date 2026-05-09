#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRBRIEF_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRBRIEF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0x10
class SelectableCar : public bTNode<SelectableCar> {
  public:
    SelectableCar(uint32 handle, bool locked)
        : mHandle(handle) //
          ,
          bLocked(locked) {}

    ~SelectableCar() {}

    uint32 mHandle; // offset 0x8, size 0x4
    bool bLocked;   // offset 0xC, size 0x1
};

// total size: 0x14
class SelectableTrack : public bTNode<SelectableTrack> {
  public:
    // static void *operator new(size_t s) {
    //     return ::operator new[](s);
    // }
    // static void operator delete(void *p) {
    //     ::operator delete[](p);
    // }

    SelectableTrack(GRaceParameters *rp, bool locked, int bin_num)
        : pRaceParams(rp) //
          ,
          bLocked(locked) //
          ,
          bin(bin_num) {}

    ~SelectableTrack() {}

    GRaceParameters *pRaceParams; // offset 0x8, size 0x4
    bool bLocked;                 // offset 0xC, size 0x1
    int bin;                      // offset 0x10, size 0x4
};

// total size: 0x138
class UIQRBrief : public MenuScreen {
  public:
    UIQRBrief(ScreenConstructorData *sd);
    ~UIQRBrief() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader();

  private:
    void Setup();
    void UpdateSliders();
    SelectableCar *GetRandomCar();
    SelectableTrack *GetRandomTrack();

    bTList<SelectableCar> FilteredCarsList;     // offset 0x2C, size 0x8
    SelectableCar *pSelectedCar;                // offset 0x34, size 0x4
    bTList<SelectableTrack> FilteredTracksList; // offset 0x38, size 0x8
    SelectableTrack *pSelectedTrack;            // offset 0x40, size 0x4
    RaceSettings raceSettings;                  // offset 0x44, size 0x24
    int randomCount;                            // offset 0x68, size 0x4
    TwoStageSlider AccelerationSlider;          // offset 0x6C, size 0x44
    TwoStageSlider TopSpeedSlider;              // offset 0xB0, size 0x44
    TwoStageSlider HandlingSlider;              // offset 0xF4, size 0x44
};

#endif
