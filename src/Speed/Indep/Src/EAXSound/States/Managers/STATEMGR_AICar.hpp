#ifndef STATEMGR_AICAR_HPP
#define STATEMGR_AICAR_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#define MAX_NUM_SND_AI_CARS 3 // Decl: 10

// total size: 0x24
// Decl: 13
class CSTATEMGR_AICar : public CSTATEMGR_CarState {
  public:
    CSTATEMGR_AICar();
    ~CSTATEMGR_AICar() override;

    // Overrides: CSTATEMGR_Base
    void UpdateParams(float t) override;
    void EnterWorld(eSndGameMode esgm) override;

    static void QueueSlots();

    static bool bUsingGinsu; // size: 0x1, address: 0x80417F38

  private:
    void DebugDisplayAIConnections();
};

#endif
