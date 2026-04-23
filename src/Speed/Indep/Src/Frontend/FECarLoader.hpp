#ifndef FRONTEND_FECARLOADER_H
#define FRONTEND_FECARLOADER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/World/CarInfo.hpp"

struct GarageCarLoader {
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

    bool IsThereALoadingRideInfo() {
        return IsLoadingRide;
    }
    bool IsThereACurrentRideInfo() {
        return IsCurrentRide;
    }
    bool HasSwitched() {
        return IsDifferent;
    }

    RideInfo LoadingRideInfo;              // offset 0x0, size 0x310
    RideInfo CurrentRideInfo;              // offset 0x310, size 0x310
    bool IsLoadingRide;                    // offset 0x620, size 0x1
    bool IsCurrentRide;                    // offset 0x624, size 0x1
    int LoadingCar;                        // offset 0x628, size 0x4
    int CurrentCar;                        // offset 0x62C, size 0x4
    bool IsDifferent;                      // offset 0x630, size 0x1
    bool UseFirstDummyTexturesForNextLoad; // offset 0x634, size 0x1
};

void InitGarageCarLoaders();
void CleanUpGarageCarLoaders();

#endif
