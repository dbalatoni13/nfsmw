#ifndef STATEMGR_TRAFFICCAR_HPP
#define STATEMGR_TRAFFICCAR_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#define MAX_NUM_TOTAL_TRAFFIC_CARS 16 // Decl: 5

#define MAX_NUM_ALLOCATED_TRAFFIC_CARS 6 // Decl: 10

// total size: 0x24
// Decl: 22
class CSTATEMGR_TrafficCar : public CSTATEMGR_CarState {
  public:
    CSTATEMGR_TrafficCar();
    ~CSTATEMGR_TrafficCar() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;

  private:
    void DebugDisplayTrafficConnections();
};

#endif
