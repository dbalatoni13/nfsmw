#ifndef __MYCARSMANAGER_HPP__
#define __MYCARSMANAGER_HPP__

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

#include <types.h>

// total size: 0x28
// Decl: speed/indep/src/frontend/menuscreens/customize/FeMyCarsManager.hpp:17
class CarDatum : public ArrayDatum {
  public:
    CarDatum(uint32 hash, uint32 desc, uint32 handle) : ArrayDatum(hash, desc), Handle(handle) {}
    ~CarDatum() override {} // Decl: speed/indep/src/frontend/menuscreens/customize/FeMyCarsManager.hpp:20

    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    uint32 Handle; // offset 0x24, size 0x4
};

// total size: 0x1BC
// Decl: speed/indep/src/frontend/menuscreens/customize/FeMyCarsManager.hpp:45
class MyCarsManager : public ArrayScrollerMenu {
  public:
    MyCarsManager(ScreenConstructorData *sd);
    ~MyCarsManager() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void Setup();
    void RefreshHeader() override;
    void RefreshCarList();
    void UpdateSliders();
    void UpdateCar();
    bool CanAddMoreCars();

    TwoStageSlider AccelerationSlider; // offset 0xE8, size 0x44
    TwoStageSlider TopSpeedSlider;     // offset 0x12C, size 0x44
    TwoStageSlider HandlingSlider;     // offset 0x170, size 0x44
    FECarRecord *pSelectedCar;         // offset 0x1B4, size 0x4
    Timer tCarLoadTimer;               // offset 0x1B8, size 0x4
    bool bGoToShowcase;                // offset 0x1BC, size 0x1
};

#endif
