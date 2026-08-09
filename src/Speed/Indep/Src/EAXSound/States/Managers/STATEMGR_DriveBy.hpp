#ifndef STATEMGR_DRIVEBY_HPP
#define STATEMGR_DRIVEBY_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

#define MAX_NUM_WOOSH 6 // Decl: 7

// total size: 0x20
// Decl: 10
class CSTATEMGR_DriveBy : public CSTATEMGR_Base {
  public:
    CSTATEMGR_DriveBy();
    ~CSTATEMGR_DriveBy() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;

    void UpdateSmackables(float t);

    int WooshCheckFrameCntr;          // offset 0x1C, size 0x4, Decl: 22
    bool ShouldPerformWooshCheck() {} // Decl: 23
    static int UpdateSmokeableWooshs; // size: 0x4, address: 0xFFFFFFFF

    // Overrides: CSTATEMGR_Base
    CSTATE_Base *GetFreeState(void *ObjectPtr) override;
};

#endif
