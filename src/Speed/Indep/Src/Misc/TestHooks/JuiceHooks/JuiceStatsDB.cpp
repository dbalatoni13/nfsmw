#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

extern unsigned int eFrameCounter;

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

void JuiceStatsDB::SubmitCareerData(const char *profileName) {
    float *pursuitTime;
    if (mShouldDumpCareerData && (pursuitTime = mCareerFloatDataDB[0] + 2, profileName != nullptr)) {
        int *pursuitLength = mCareerIntDataDB[0] + 2;
        int *cashEarned = mCareerIntDataDB[0] + 1;
        int (*repEarned)[3] = mCareerIntDataDB;
        int offset = 0;
        float zero = 0.0f;
        do {
            Juice::GameHook::Instance()->LogText(
                "[EVENT LOGGED] - CAREER_TRACKING\n");
            if ((*repEarned)[0] != 0) {
                Juice::GameHook::Instance()->GameEvent(
                    "CAREER_TRACKING", "REP_EARNED", "", (*repEarned)[0], 0.0f,
                    const_cast<char *>(profileName), "", "");
            }
            if (*cashEarned != 0) {
                Juice::GameHook::Instance()->GameEvent(
                    "CAREER_TRACKING", "CASH_EARNED", "", *cashEarned, 0.0f,
                    const_cast<char *>(profileName), "", "");
            }
            if (static_cast<float>(*pursuitLength) != zero) {
                Juice::GameHook::Instance()->GameEvent(
                    "CAREER_TRACKING", "PURSUIT_LENGTH", "",
                    0, *reinterpret_cast<float *>(reinterpret_cast<char *>(pursuitTime) + offset),
                    const_cast<char *>(profileName), "", "");
            }
            pursuitLength += 3;
            cashEarned += 3;
            repEarned++;
            offset += 0xc;
        } while (pursuitLength < reinterpret_cast<int *>(pursuitTime));
        ResetCareerData();
    }
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

void JuiceStatsDB::IncrementPerRaceStat(JuicePerRaceStatType statType) {
    if (statType == JUICE_COP_CHOPPER_UNSPAWN) {
        goto chopper;
    }
    if (statType < JUICE_TRAFFIC_SPAWN) {
        if (statType == JUICE_COP_CAR_UNSPAWN) {
            goto car;
        }
    } else if (statType == JUICE_TRAFFIC_UNSPAWN) {
        goto traffic;
    }
    goto increment;

car:
    if (mPerRaceStatsDB[0] == 0) {
        return;
    }
    if (mFrameOfLastInc[0] == eFrameCounter) {
        mPerRaceStatsDB[0]--;
        return;
    }
    goto increment;

chopper:
    if (mPerRaceStatsDB[2] == 0) {
        return;
    }
    if (mFrameOfLastInc[2] == eFrameCounter) {
        mPerRaceStatsDB[2]--;
        return;
    }
    goto increment;

traffic:
    if (mPerRaceStatsDB[4] == 0) {
        return;
    }
    if (mFrameOfLastInc[4] == eFrameCounter) {
        mPerRaceStatsDB[4]--;
        return;
    }

increment:
    if (!mShouldDumpStats) {
        mShouldDumpStats = true;
    }
    mPerRaceStatsDB[statType]++;
    mFrameOfLastInc[statType] = eFrameCounter;
}

void JuiceStatsDB::UpdateTimers() {
    float elapsed = bGetTickerDifference(mCurrentTicker);
    if (elapsed > 500.0f) {
        elapsed = bGetTickerDifference(mCurrentTicker);
        bool racing = TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING;
        mTimeAtHeatLevel[mPerRaceStatsDB[JUICE_CURRENT_HEAT]] += elapsed * 0.001f;
        if (racing) {
            elapsed = bGetTickerDifference(mCurrentTicker);
            unsigned int start_ticks = mCurrentTicker;
            mTimeInCameraMode[mPerRaceStatsDB[JUICE_CURRENT_CAMERA]] += elapsed * 0.001f;
            elapsed = bGetTickerDifference(start_ticks);
            mTimeInRaceType[mPerRaceStatsDB[JUICE_CURRENT_RACE]] += elapsed * 0.001f;
        }
        mCurrentTicker = bGetTicker();
    }
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
