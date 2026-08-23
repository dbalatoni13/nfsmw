#include "../../../include/common/realcore/input.h"

#include "imem.h"

namespace RealInput {

Device::Device(Platform platform, Type type) {
    this->InitData();
    this->mInfo.mPlatform = platform;
    this->mInfo.mType = type;
}

Device::~Device() {}

void Device::InitData() {
    this->mInfo.mPlatform = PLATFORM_WIN;
    this->mInfo.mType = TYPE_UNKNOWN;
    IMem::Clear(&this->mInfo, sizeof(this->mInfo));
    IMem::Clear(&this->mCapabilities, sizeof(this->mCapabilities));
}

} // namespace RealInput
