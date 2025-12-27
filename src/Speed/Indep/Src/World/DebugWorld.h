#ifndef WORLD_DEBUGWORLD_H
#define WORLD_DEBUGWORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <types.h>

// total size: 0x1
class DebugWorld {
  public:
    static void Init();

    DebugWorld();
    void Service();
    void HandleSaveHotPosition();
    void HandleJumpToHotPosition();

    static DebugWorld &Get() {
        return *mThis;
    }

    static bool Exists() {
        return mThis != nullptr;
    }

  private:
    static DebugWorld *mThis;
    static bool mOnOff;
};

struct SavedHotPosition {
    // total size: 0x18
    bVector3 Position;    // offset 0x0, size 0x10
    unsigned short Angle; // offset 0x10, size 0x2
    float Speed;          // offset 0x14, size 0x4

    SavedHotPosition() {}
};

#endif
