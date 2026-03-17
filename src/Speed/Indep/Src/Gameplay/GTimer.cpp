#include "Speed/Indep/Src/Gameplay/GTimer.h"

#include "Speed/Indep/Src/Generated/Messages/MNotifyTimer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

GTimer::GTimer() {
    Reset(0.0f);
}

GTimer::~GTimer() {}

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
    if (!mRunning) {
        return mTotalTime;
    }

    return mTotalTime + (Sim::GetTime() - mStartTime);
}

void GTimer::SetTime(float time) {
    mTotalTime = time;
    mRunning = false;
}

GEventTimer::GEventTimer() {
    Reset();
}

GEventTimer::~GEventTimer() {}

void GEventTimer::Reset() {
    mInterval = 1.0f;
    mRunning = false;
    mElapsed = 0.0f;
    mNameHash = 0;
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
    if (mRunning != 0) {
        mElapsed += dT;
        if (mInterval <= mElapsed) {
            MNotifyTimer(mName).Post(UCrc32(0x20D60DBF));
            mElapsed -= mInterval;
        }
    }
}
