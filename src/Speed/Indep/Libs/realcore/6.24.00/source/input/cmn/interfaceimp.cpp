#include "interfaceimp.h"

#include "eventqueue.h"
#include "imem.h"

namespace RealInput {

extern Interface *gInterface;

InterfaceImp::InterfaceImp(const ConfigOptions &options)
    : mConfigOptions()
    , mRefcount(1)
    , mpEventQueue(nullptr)
    , mpPad(nullptr)
    , mpKeyboard(nullptr)
    , mpMouse(nullptr)
    , mNumDevices(0) {
    this->mConfigOptions = options;
    IMem::Clear(this->mpDeviceList, sizeof(this->mpDeviceList));
    this->mNumDevices = 0;
    this->mpEventQueue = new EventQueue(this->mConfigOptions.mEventQueueSize);
}

InterfaceImp::~InterfaceImp() {
    if (this->mpEventQueue != nullptr) {
        delete this->mpEventQueue;
    }
    do {
        if (this->mpDeviceList[0] != nullptr) {
            delete this->mpDeviceList[0];
        }
    } while (this->mNumDevices > 0);
    this->mNumDevices = 0;
}

int InterfaceImp::AddRef() {
    this->mRefcount++;
    return this->mRefcount;
}

int InterfaceImp::Release() {
    this->mRefcount--;
    if (this->mRefcount == 0) {
        if (this == gInterface) {
            gInterface = nullptr;
        }
        if (this != nullptr) {
            delete this;
        }
        ReleaseAllocator();
        return 0;
    }
    return this->mRefcount;
}

void InterfaceImp::Update() {
    if (this->mpEventQueue != nullptr) {
        this->mpEventQueue->Clear();
    }

    DeviceImp **pD = this->mpDeviceList;
    for (int i = 0; i < this->mNumDevices; i++) {
        if (*pD != nullptr) {
            (*pD)->Update();
        }
        pD++;
    }
}

Event *InterfaceImp::GetEvent() {
    if (this->mpEventQueue != nullptr) {
        return this->mpEventQueue->GetEvent();
    }
    return nullptr;
}

bool InterfaceImp::RegisterDevice(DeviceImp *pDevice) {
    if (this->mNumDevices < 32) {
        this->mpDeviceList[this->mNumDevices++] = pDevice;

        switch (pDevice->GetInfo()->mType) {
        case Device::TYPE_UNKNOWN:
            break;
        case Device::TYPE_PAD:
            this->mpPad = pDevice;
            break;
        case Device::TYPE_KEYBOARD:
            this->mpKeyboard = pDevice;
            break;
        case Device::TYPE_MOUSE:
            this->mpMouse = pDevice;
            break;
        }
        return true;
    }
    return false;
}

bool InterfaceImp::UnRegisterDevice(DeviceImp *pDevice) {
    if (pDevice == nullptr) {
        return false;
    }

    DeviceImp **pD = this->mpDeviceList;
    for (int i = 0; i < this->mNumDevices; i++, pD++) {
        if (*pD == pDevice) {
            this->mNumDevices--;
            *pD = this->mpDeviceList[this->mNumDevices];
            if (pDevice == this->mpPad) {
                this->mpPad = nullptr;
            }
            if (pDevice == this->mpKeyboard) {
                this->mpKeyboard = nullptr;
            }
            if (pDevice == this->mpMouse) {
                this->mpMouse = nullptr;
            }
            return true;
        }
    }
    return false;
}

} // namespace RealInput
