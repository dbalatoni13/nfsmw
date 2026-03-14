// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#ifndef FRONTEND_FECARLOADER_H
#define FRONTEND_FECARLOADER_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include <types.h>
#include "Speed/Indep/Src/World/CarInfo.hpp"
struct GarageCarLoader {
    bool IsThereALoadingRideInfo() { return IsLoadingRide; }
    bool IsThereACurrentRideInfo() { return IsCurrentRide; }
    bool HasSwitched() { return IsDifferent; }
    GarageCarLoader();
    ~GarageCarLoader();
    void Init();
    void CleanUp();
    void CancelCarLoad();
    void LoadRideInfo(RideInfo *ride_info);
    RideInfo *GetLoadingRideInfo();
    RideInfo *GetCurrentRideInfo();
    void Switch();
    void Update();
    char _pad_ride0[0x310];                   // offset 0x0 (RideInfo placeholder)
    char _pad_ride1[0x310];                   // offset 0x310 (RideInfo placeholder)
    bool IsLoadingRide;                       // offset 0x620
    bool IsCurrentRide;                       // offset 0x624
    int LoadingCar;                           // offset 0x628
    int CurrentCar;                           // offset 0x62C
    bool IsDifferent;                         // offset 0x630
    bool UseFirstDummyTexturesForNextLoad;    // offset 0x634
};
#endif
