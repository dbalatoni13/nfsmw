#ifndef FECARLOADER_H
#define FECARLOADER_H

#include <types.h>
#include "Speed/Indep/Src/World/CarInfo.hpp"

// total size: 0x638
// Decl: 9
class GarageCarLoader {
  private:
    friend void InitGarageCarLoaders();
    void Init(); // Decl: 15

  public:
    bool IsThereALoadingRideInfo() { // Decl: 17
        return IsLoadingRide;
    }

    bool IsThereACurrentRideInfo() { // Decl: 18
        return IsCurrentRide;
    }

    RideInfo *GetLoadingRideInfo(); // Decl: 20

    RideInfo *GetCurrentRideInfo(); // Decl: 21

    bool HasSwitched() { // Decl: 25
        return IsDifferent;
    }

    void Switch(); // Decl: 26

    void CancelCarLoad(); // Decl: 28

    void LoadRideInfo(RideInfo *ride_info); // Decl: 30

    void Update(); // Decl: 32

    GarageCarLoader(); // Decl: 35

    ~GarageCarLoader(); // Decl: 36

  private:
    friend void CleanUpGarageCarLoaders();
    void CleanUp(); // Decl: 40

    RideInfo LoadingRideInfo;              // offset 0x0, size 0x310, Decl: 43
    RideInfo CurrentRideInfo;              // offset 0x310, size 0x310, Decl: 44
    bool IsLoadingRide;                    // offset 0x620, size 0x1, Decl: 48
    bool IsCurrentRide;                    // offset 0x624, size 0x1, Decl: 49
    CarLoaderHandle LoadingCar;            // offset 0x628, size 0x4, Decl: 51
    CarLoaderHandle CurrentCar;            // offset 0x62C, size 0x4, Decl: 52
    bool IsDifferent;                      // offset 0x630, size 0x1, Decl: 54
    bool UseFirstDummyTexturesForNextLoad; // offset 0x634, size 0x1
};

void InitGarageCarLoaders();
void CleanUpGarageCarLoaders();
void UpdateGarageCarLoaders();
GarageCarLoader *GetGarageCarLoader();

#endif
