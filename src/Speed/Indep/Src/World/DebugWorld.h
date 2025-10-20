#ifndef WORLD_DEBUGWORLD_H
#define WORLD_DEBUGWORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "dolphin/types.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct DebugWorld {
    // total size: 0x1
    public:
    DebugWorld();

    void Init();
    void Service();
    void HandleSaveHotPosition();
    void HandleJumpToHotPosition();
    
    static DebugWorld *mThis;
    static bool mOnOff;

    static DebugWorld &Get() {
        return *mThis;
    }

    static bool Exists() {
        return mThis != nullptr;
    }
};

struct SavedHotPosition {
    // total size: 0x18
    bVector3 Position; // offset 0x0, size 0x10
    unsigned short Angle; // offset 0x10, size 0x2
    float Speed; // offset 0x14, size 0x4

    SavedHotPosition() {}
};

#endif
