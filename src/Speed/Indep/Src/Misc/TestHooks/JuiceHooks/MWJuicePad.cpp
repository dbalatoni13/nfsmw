#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

MWJuicePad *MWJuicePad::mInstance;

MWJuicePad *MWJuicePad::Instance() {
    if (mInstance == nullptr) {
        mInstance = new ("MWJuicePad", 0) MWJuicePad();
    }
    return mInstance;
}

MWJuicePad::MWJuicePad() {
    mInputQueue = new ActionQueue(0, 0x82D21520, "juiceinput", false);
    mInputQueue->Enable(true);
    mIsLastFrame = false;
    mIsInBE = false;
    mResetSegmentPresses = false;
}

void MWJuicePad::ResetGamePad() {
    mResetSegmentPresses = true;
}

void MWJuicePad::SetIsLastFrame(bool val) {
    mIsLastFrame = val;
}

bool MWJuicePad::IsLastJoyFrame() {
    return mIsLastFrame;
}
