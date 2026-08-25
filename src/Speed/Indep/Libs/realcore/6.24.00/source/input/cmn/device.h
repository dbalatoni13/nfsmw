#ifndef REALCORE_SOURCE_INPUT_CMN_DEVICE_H
#define REALCORE_SOURCE_INPUT_CMN_DEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/input.h"

namespace RealInput {

struct DeviceImp : Device {
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
