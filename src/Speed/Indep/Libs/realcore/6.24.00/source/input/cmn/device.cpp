#include "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/device.h"

void MEM_fill(void *vdst, unsigned int fill, int size);

RealInput::Device::Device(RealInput::Platform platform, RealInput::Device::Type type) {
    InitData();
    mPlatform = platform;
    mType = type;
}

RealInput::Device::~Device() {
}

void RealInput::Device::InitData() {
    mPlatform = 0;
    mType = 0;
    MEM_fill(this, 0, 0x14);
    MEM_fill(&mStat, 0, 0xC);
}

void *RealInput::Device::GetData() {
    return mData;
}

int RealInput::Device::Acquire() {
    return 0;
}

int RealInput::Device::Release() {
    return 0;
}

int RealInput::Device::Update() {
    return 0;
}

RealInput::Effect *RealInput::Device::CreateEffect(RealInput::Effect::Info *info) {
    return 0;
}

RealInput::Effect *RealInput::Device::GetEffect() {
    return 0;
}

int RealInput::Device::GetKeyState(unsigned int key) {
    return 0;
}
