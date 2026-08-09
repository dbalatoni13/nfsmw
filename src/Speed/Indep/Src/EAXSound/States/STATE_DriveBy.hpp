#ifndef STATE_DRIVEBY_HPP
#define STATE_DRIVEBY_HPP

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x20
// Decl: 9
struct stDriveByInfo {
    // Decl: 10
    stDriveByInfo() {
        this->eDriveByType = DRIVE_BY_LAMPPOST;
        this->pEAXCar = nullptr;
        this->ClosingVelocity = 0.0f;
        this->vLocation = bVector3(0.0f, 0.0f, 0.0f);
        this->UniqueID = 0;
    }

    eDRIVE_BY_TYPE eDriveByType; // offset 0x0, size 0x4, Decl: 19
    EAXCar *pEAXCar;             // offset 0x4, size 0x4, Decl: 20
    float ClosingVelocity;       // offset 0x8, size 0x4, Decl: 21
    bVector3 vLocation;          // offset 0xC, size 0x10, Decl: 22
    uintptr_t UniqueID;          // offset 0x1C, size 0x4, Decl: 23
};

// total size: 0x64
// Decl: 29
class CSTATE_DriveBy : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_DriveBy();
    ~CSTATE_DriveBy() override;

    // Overrides: CSTATE_Base
    void Attach(void *pAttachment) override;
    bool Detach() override;

    stDriveByInfo m_DriveByInfo; // offset 0x44, size 0x20, Decl: 39
};

#endif
