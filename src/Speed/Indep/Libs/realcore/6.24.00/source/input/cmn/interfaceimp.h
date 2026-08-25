#ifndef REALCORE_SOURCE_INPUT_CMN_INTERFACEIMP_H
#define REALCORE_SOURCE_INPUT_CMN_INTERFACEIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/input.h"

namespace RealInput {

struct InterfaceImp : Interface {
    InterfaceImp(const ConfigOptions &options);
    virtual ~InterfaceImp();

    virtual int AddRef();
    virtual int Release();
    virtual void Update();
    virtual Device *GetPad();
    virtual Device *GetMouse();
    virtual Device *GetKeyboard();
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
