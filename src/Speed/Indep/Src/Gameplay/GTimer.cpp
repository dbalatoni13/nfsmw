#include "GTimer.h"

#include "Speed/Indep/Src/Generated/Messages/MNotifyTimer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

GTimer::GTimer() {
    Reset(0.0f);
}

GTimer::~GTimer() {
}

void GTimer::Start() {
    mTotalTime = GetTime();
    mStartTime = Sim::GetTime();
    mRunning = true;
}

void GTimer::Stop() {
    mTotalTime = GetTime();
    mRunning = false;
}

void GTimer::Reset(float value) {
    mTotalTime = value;
    mStartTime = Sim::GetTime();
}

float GTimer::GetTime() const {
    if (mRunning) {
        float elapsed = Sim::GetTime() - mStartTime;
        return mTotalTime + elapsed;
    }
    return mTotalTime;
}

void GTimer::SetTime(float time) {
    mTotalTime = time;
    mRunning = false;
}

GEventTimer::GEventTimer() {
    Reset();
}

GEventTimer::~GEventTimer() {
}

void GEventTimer::Reset() {
    mElapsed = 0.0f;
    mRunning = false;
    mInterval = 1.0f;
    mNameHash = 0;
    mName[0] = '\0';
}

void GEventTimer::Start() {
    mRunning = true;
    mElapsed = 0.0f;
}

void GEventTimer::Stop() {
    mRunning = false;
    mElapsed = 0.0f;
}

void GEventTimer::SetInterval(float value) {
    mInterval = value;
    mElapsed = 0.0f;
}

void GEventTimer::Update(float dT) {
    if (mRunning) {
        mElapsed += dT;
        if (mElapsed >= mInterval) {
            MNotifyTimer(mName).Post(0x20d60dbf);
            mElapsed -= mInterval;
        }
    }
}

void GEventTimer::SetName(const char *name) {
    bSafeStrCpy(mName, name, 0x13);
    mNameHash = bStringHash(mName);
}

void GEventTimer::Serialize(SavedTimerInfo *saveInfo) {
    saveInfo->mElapsed = mElapsed;
    saveInfo->mInterval = mInterval;
    saveInfo->mRunning = mRunning;
    bSafeStrCpy(saveInfo->mName, mName, 0x13);
}

void GEventTimer::Deserialize(SavedTimerInfo *saveInfo) {
    mElapsed = saveInfo->mElapsed;
    mInterval = saveInfo->mInterval;
    mRunning = saveInfo->mRunning;
    bSafeStrCpy(mName, saveInfo->mName, 0x13);
    mNameHash = bStringHash(mName);
}
