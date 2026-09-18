#ifndef REALCORE_SOURCE_INPUT_CMN_DEVICEIMP_H
#define REALCORE_SOURCE_INPUT_CMN_DEVICEIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#pragma interface

#include "../../../include/common/realcore/input.h"

namespace RealInput {

struct DeviceImp : Device {
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    DeviceImp(Platform platform, Type type)
        : Device(platform, type) {
    }

    virtual ~DeviceImp() {}

    virtual RiResult Acquire() {
        return RI_OK;
    }

    virtual RiResult Release() {
        return RI_OK;
    }

    virtual RiResult Update() {
        return RI_OK;
    }
};

} // namespace RealInput

#endif
