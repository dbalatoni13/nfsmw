#include "gc_effect.h"

#include "gc_device.h"

#include "../../../include/common/realcore/std.h"

#include <dolphin/pad.h>

namespace RealInput {

Effect::Info::Info()
    : mFullStop(0) {
}

GcEffect::GcEffect()
    : EffectImp()
    , mPlaying(false)
    , mPort(0)
    , mUseFullStop(0) {
}

GcEffect::~GcEffect() {
    if (this->mPlaying) {
        this->Stop();
    }
}

void GcEffect::Init(Device *pDevice, Info *pInfo) {
    EffectImp::Init(pDevice, pInfo);
    this->mPlaying = false;
    this->mPort = static_cast<GcDevice *>(pDevice)->GetPortNum();
    this->mUseFullStop = pInfo->mFullStop;
    this->SetInfo(pInfo);
}

void GcEffect::Release() {
    EffectImp::Release();
}

void GcEffect::Start() {
    PADControlMotor(this->mPort, 1);
    this->mPlaying = true;
}

void GcEffect::Stop() {
    if (this->mUseFullStop) {
        PADControlMotor(this->mPort, 2);
    } else {
        PADControlMotor(this->mPort, 0);
    }
    this->mPlaying = false;
}

Effect::Status GcEffect::GetStatus() {
    if (this->mPlaying) {
        this->mStatus = STATUS_PLAYING;
    } else {
        this->mStatus = STATUS_STOPPED;
    }
    return this->mStatus;
}

void GcEffect::SetInfo(Info *pInfo) {
    MEM_copy(&this->mInfo, pInfo, sizeof(Info));
    this->mUseFullStop = pInfo->mFullStop;
    if (this->mPlaying) {
        this->Start();
    }
}

} // namespace RealInput
