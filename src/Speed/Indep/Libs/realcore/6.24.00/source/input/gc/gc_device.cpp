#include "gc_device.h"

namespace RealInput {

GcDevice::GcDevice(Type type, GcInterface *pInterface)
    : DeviceImp(PLATFORM_XBOX, type) {
    this->mpInterface = pInterface;
}

int GcDevice::GetPortNum() {
    return this->mPortNum;
}

void GcDevice::SetPortNum(int port) {
    this->mPortNum = port;
}

GcDevice::~GcDevice() {}

} // namespace RealInput
