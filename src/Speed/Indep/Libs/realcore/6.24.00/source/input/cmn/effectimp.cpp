#include "effectimp.h"

#include "../../../include/common/realcore/std.h"

namespace RealInput {

EffectImp::EffectImp()
    : mInfo()
    , mpDevice(nullptr)
    , mStatus(STATUS_STOPPED)
    , mInUse(false) {
}

void EffectImp::Init(Device *pDevice, Info *pInfo) {
    this->mpDevice = pDevice;
    this->mStatus = STATUS_STOPPED;
    this->mInfo = *pInfo;
    this->mInUse = true;
}

void EffectImp::Release() {
    this->mInUse = false;
}

EffectImp::~EffectImp() {}

void EffectImp::Start() {}

void EffectImp::Stop() {}

Effect::Status EffectImp::GetStatus() {
    return this->mStatus;
}

Device *EffectImp::GetDevice() {
    return this->mpDevice;
}

void EffectImp::GetInfo(Info *pInfo) {
    if (pInfo != nullptr) {
        MEM_copy(pInfo, &this->mInfo, sizeof(Info));
    }
}

void EffectImp::SetInfo(Info *) {}

} // namespace RealInput
