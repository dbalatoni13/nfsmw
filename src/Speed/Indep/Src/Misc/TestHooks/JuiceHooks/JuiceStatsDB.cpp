#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

JuiceStatsDB *JuiceStatsDB::mInstance;

JuiceStatsDB *JuiceStatsDB::Instance() {
    if (mInstance == nullptr) {
        mInstance = new ("JuiceStatsDB", 0) JuiceStatsDB();
    }
    return mInstance;
}

void JuiceStatsDB::ResetPerRaceStats() {
    for (int index = 0; index < 10; index++) {
        mPerRaceStatsDB[index] = 0;
        mFrameOfLastInc[index] = 0;
    }
    for (int index = 0; index < 11; index++) {
        mTimeAtHeatLevel[index] = 0.0f;
    }
    for (int index = 0; index < 6; index++) {
        mTimeInCameraMode[index] = 0.0f;
        mTimeInRaceType[index] = 0.0f;
    }
    mShouldDumpStats = false;
}

void JuiceStatsDB::SetHeat(int heat) {
    if (heat != mPerRaceStatsDB[JUICE_CURRENT_HEAT]) {
        mShouldDumpStats = true;
        mPerRaceStatsDB[JUICE_CURRENT_HEAT] = heat;
        if (mPerRaceStatsDB[JUICE_MAX_HEAT] < heat) {
            mPerRaceStatsDB[JUICE_MAX_HEAT] = heat;
        }
    }
}

void JuiceStatsDB::SetCamera(int camID) {
    if (camID != mPerRaceStatsDB[JUICE_CURRENT_CAMERA]) {
        mPerRaceStatsDB[JUICE_CURRENT_CAMERA] = camID;
        mShouldDumpStats = true;
    }
}
