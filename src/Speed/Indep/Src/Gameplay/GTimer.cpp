#include "Speed/Indep/Src/Gameplay/GTimer.h"

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
