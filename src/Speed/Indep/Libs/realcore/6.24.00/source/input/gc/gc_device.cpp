#pragma implementation "deviceimp.h"
#include "gc_device.h"

namespace RealInput {

GcDevice::GcDevice(Type type, GcInterface *pInterface)
    : DeviceImp(PLATFORM_XBOX, type) {
    this->mpInterface = pInterface;
}

GcDevice::~GcDevice() {}

int GcDevice::GetPortNum() {
    return this->mPortNum;
}

void GcDevice::SetPortNum(int port) {
    this->mPortNum = port;
}

} // namespace RealInput
