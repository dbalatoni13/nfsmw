#ifndef REALCORE_SOURCE_INPUT_CMN_INTERFACEIMP_H
#define REALCORE_SOURCE_INPUT_CMN_INTERFACEIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/input.h"

#include "device.h"

namespace RealInput {

struct InterfaceImp : Interface {
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    InterfaceImp(const ConfigOptions &options);
    virtual ~InterfaceImp();

    virtual int AddRef();
    virtual int Release();
    virtual void Update();
    virtual Device *GetPad() {
        return this->mpPad;
    }

    virtual Device *GetMouse() {
        return this->mpMouse;
    }

    virtual Device *GetKeyboard() {
        return this->mpKeyboard;
    }
    virtual Event *GetEvent();

    bool RegisterDevice(DeviceImp *device);
    bool UnRegisterDevice(DeviceImp *device);

    EventQueue *GetEventQueue() {
        return this->mpEventQueue;
    }

    ConfigOptions *GetConfigOptions() {
        return &this->mConfigOptions;
    }

    ConfigOptions mConfigOptions;
    int mRefcount;
    DeviceImp *mpDeviceList[32];
    int mNumDevices;
    DeviceImp *mpMouse;
    DeviceImp *mpKeyboard;
    DeviceImp *mpPad;
    EventQueue *mpEventQueue;
};

} // namespace RealInput

#endif
