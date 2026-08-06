#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

JuiceStatsDB *JuiceStatsDB::mInstance;

JuiceStatsDB *JuiceStatsDB::Instance() {
    if (mInstance == nullptr) {
        mInstance = new ("JuiceStatsDB", 0) JuiceStatsDB();
    }
    return mInstance;
}

JuiceStatsDB::JuiceStatsDB() {
    int index = 1;
    mCurrentOptions.TheVideoSettings.Default();
    mCurrentOptions.TheGameplaySettings.Default();
    mCurrentOptions.TheAudioSettings.Default();
    PlayerSettings *playerSettings = mCurrentOptions.ThePlayerSettings;
    do {
        index--;
        playerSettings->Default();
        playerSettings++;
    } while (index != -1);
    mCurrentOptions.Default();
    ResetPerRaceStats();
    ResetCareerData();
}

void JuiceStatsDB::ResetCareerData() {
    int row = 0;
    int *intBase = mCareerIntDataDB[0];
    float *floatBase = mCareerFloatDataDB[0];
    do {
        int column = 2;
        float *floatData = reinterpret_cast<float *>(row * 0xc + reinterpret_cast<char *>(floatBase));
        int *intData = reinterpret_cast<int *>(row * 0xc + reinterpret_cast<char *>(intBase));
        do {
            *intData = 0;
            column--;
            *floatData = 0.0f;
            intData++;
            floatData++;
        } while (column >= 0);
        row++;
    } while (row < 0xf);
    mShouldDumpCareerData = false;
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
    }
    for (int index = 0; index < 6; index++) {
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
