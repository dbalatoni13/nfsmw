#ifndef GAMEPLAY_GCHARACTER_H
#define GAMEPLAY_GCHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"

// total size: 0x80
class GCharacter : public GRuntimeInstance, public UTL::COM::Object, public IAttachable {
  public:
    GCharacter(const Attrib::Key &triggerKey);

  private:
    UMath::Vector3 mSpawnPos;           // offset 0x40, size 0xC
    unsigned char mState;               // offset 0x4C, size 0x1
    unsigned char mFlags;               // offset 0x4D, size 0x1
    unsigned short mCreateAttemptsMade; // offset 0x4E, size 0x2
    UMath::Vector3 mSpawnDir;           // offset 0x50, size 0xC
    float mSpawnSpeed;                  // offset 0x5C, size 0x4
    UMath::Vector3 mTargetPos;          // offset 0x60, size 0xC
    IVehicle *mVehicle;                 // offset 0x6C, size 0x4
    UMath::Vector3 mTargetDir;          // offset 0x70, size 0xC
    Sim::Attachments *mAttachments;     // offset 0x7C, size 0x4
};

DECLARE_CONTAINER_TYPE(ID_GCharacterList);

#endif
