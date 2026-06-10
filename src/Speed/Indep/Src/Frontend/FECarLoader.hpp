#ifndef FECARLOADER_H
#define FECARLOADER_H

#include <types.h>
#include "Speed/Indep/Src/World/CarInfo.hpp"

// total size: 0x638
// Decl: speed/indep/src/frontend/FECarLoader.hpp:9
class GarageCarLoader {
  private:
    void Init(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:15

  public:
    bool IsThereALoadingRideInfo() { // Decl: speed/indep/src/frontend/FECarLoader.hpp:17
        return IsLoadingRide;
    }

    bool IsThereACurrentRideInfo() { // Decl: speed/indep/src/frontend/FECarLoader.hpp:18
        return IsCurrentRide;
    }

    RideInfo *GetLoadingRideInfo(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:20

    RideInfo *GetCurrentRideInfo(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:21

    bool HasSwitched() { // Decl: speed/indep/src/frontend/FECarLoader.hpp:25
        return IsDifferent;
    }

    void Switch(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:26

    void CancelCarLoad(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:28

    void LoadRideInfo(RideInfo *ride_info); // Decl: speed/indep/src/frontend/FECarLoader.hpp:30

    void Update(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:32

    GarageCarLoader(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:35

    ~GarageCarLoader(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:36

  private:
    void CleanUp(); // Decl: speed/indep/src/frontend/FECarLoader.hpp:40

    RideInfo LoadingRideInfo;              // offset 0x0, size 0x310, Decl: speed/indep/src/frontend/FECarLoader.hpp:43
    RideInfo CurrentRideInfo;              // offset 0x310, size 0x310, Decl: speed/indep/src/frontend/FECarLoader.hpp:44
    bool IsLoadingRide;                    // offset 0x620, size 0x1, Decl: speed/indep/src/frontend/FECarLoader.hpp:48
    bool IsCurrentRide;                    // offset 0x624, size 0x1, Decl: speed/indep/src/frontend/FECarLoader.hpp:49
    CarLoaderHandle LoadingCar;            // offset 0x628, size 0x4, Decl: speed/indep/src/frontend/FECarLoader.hpp:51
    CarLoaderHandle CurrentCar;            // offset 0x62C, size 0x4, Decl: speed/indep/src/frontend/FECarLoader.hpp:52
    bool IsDifferent;                      // offset 0x630, size 0x1, Decl: speed/indep/src/frontend/FECarLoader.hpp:54
    bool UseFirstDummyTexturesForNextLoad; // offset 0x634, size 0x1
};

void InitGarageCarLoaders();
void CleanUpGarageCarLoaders();
void UpdateGarageCarLoaders();

#endif
