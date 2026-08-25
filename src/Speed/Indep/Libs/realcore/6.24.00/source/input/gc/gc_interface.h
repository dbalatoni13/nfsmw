#ifndef REALCORE_SOURCE_INPUT_GC_INTERFACE_H
#define REALCORE_SOURCE_INPUT_GC_INTERFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../cmn/interfaceimp.h"

namespace RealInput {

struct GcInterface : InterfaceImp {
    GcInterface(const ConfigOptions &options);
    virtual ~GcInterface();

    static void *operator new(unsigned int size) {
        return AllocateMemSize(nullptr, static_cast<int>(size), 0, 0, 0);
    }

    GcEffect *GetUnusedEffectSlot();
    RiResult EnumerateDevices();

    GcEffect *mpEffectTable;
};

} // namespace RealInput

#endif
