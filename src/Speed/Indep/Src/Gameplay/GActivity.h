#ifndef GAMEPLAY_GACTIVITY_H
#define GAMEPLAY_GACTIVITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GHandler.h"
#include "GState.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

DECLARE_CONTAINER_TYPE(ID_GHandlerVector);
DECLARE_CONTAINER_TYPE(ID_StateToVectors);

typedef UTL::Std::map<GState *, UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>, _type_ID_StateToVectors> StateToHandlers;

// total size: 0x48
class GActivity : public GRuntimeInstance {
  public:
    // total size: 0x8
    struct SerializedHeader {
        unsigned int mStateNameHash; // offset 0x0, size 0x4
        unsigned short mFlags;       // offset 0x4, size 0x2
        unsigned short mTableBytes;  // offset 0x6, size 0x2
    };

    GActivity(const Attrib::Key &activityKey);

  private:
    GState *mCurrentState;            // offset 0x28, size 0x4
    GState *mRegisteredHandlersState; // offset 0x2C, size 0x4
    StateToHandlers mStateHandlers;   // offset 0x30, size 0x10
    bool mRunning;                    // offset 0x40, size 0x1
    bool mVarsInLuaVM;                // offset 0x44, size 0x1
};

#endif
