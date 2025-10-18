#ifndef WORLD_DEBUGWORLD_H
#define WORLD_DEBUGWORLD_H

#include "dolphin/types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct DebugWorld {
    // total size: 0x1
    public:
    void Service();
    
    static DebugWorld *mThis;

    static DebugWorld &Get() {
        return *mThis;
    }

    static bool Exists() {
        return mThis != nullptr;
    }
};

#endif
