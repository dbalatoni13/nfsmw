#ifndef REALCORE_SOURCE_INPUT_GC_DEVICE_H
#define REALCORE_SOURCE_INPUT_GC_DEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../cmn/device.h"

namespace RealInput {

struct GcInterface;

struct GcDevice : DeviceImp {
    static void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    GcDevice(Type type, GcInterface *pInterface);
    virtual ~GcDevice();

    int GetPortNum();
    void SetPortNum(int port);

    GcInterface *mpInterface;
    int mPortNum;
};

} // namespace RealInput

#endif
