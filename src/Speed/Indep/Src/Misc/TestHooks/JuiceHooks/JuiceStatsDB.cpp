#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

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

void JuiceStatsDB::SubmitStats(JuiceRaceType raceType) {
    char raceTypeString[64];
    float *cameraTime;
    float *raceTime;
    float *heatTime;
    if (mShouldDumpStats) {
        switch (raceType) {
        case JUICE_CIRCUIT:
            bStrCpy(raceTypeString, "CIRCUIT");
            break;
        case JUICE_FREE_ROAM:
            bStrCpy(raceTypeString, "FREE_ROAM");
            break;
        case JUICE_P2P:
            bStrCpy(raceTypeString, "P2P");
            break;
        case JUICE_OTHERRACE:
            bStrCpy(raceTypeString, "OTHERRACE");
            break;
        }

        {
            int counter = 0;
            cameraTime = mTimeInCameraMode;
            raceTime = mTimeInRaceType;
            Juice::GameHook::Instance()->LogText(
                "[STAT LOGGED] - COP_STATS, TRAFFIC_STATS, HEAT_STATS AND CAMERA_");

            heatTime = mTimeAtHeatLevel;
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "COP_CARS_SPAWNED", mPerRaceStatsDB[0]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "COP_CARS_UNSPAWNED", mPerRaceStatsDB[1]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "TRAFFIC_SPAWNED", mPerRaceStatsDB[4]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "TRAFFIC_UNSPAWNED", mPerRaceStatsDB[5]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "COP_CHOPPERS_SPAWNED", mPerRaceStatsDB[2]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "COP_CHOPPERS_UNSPAWNED", mPerRaceStatsDB[3]);
            Juice::GameHook::Instance()->LogStat(
                4, raceTypeString, "MAX_HEAT", mPerRaceStatsDB[7]);

            do {
                if (*heatTime > 0.0f) {
                    Juice::GameHook::Instance()->LogText(
                        "[EVENT LOGGED] - HEAT_LEVEL_TIME\n");
                    Juice::GameHook::Instance()->GameEvent(
                        "HEAT_LEVEL_TIME", raceTypeString, "", counter, *heatTime, "", "", "");
                }
                counter++;
                heatTime++;
            } while (counter < 11);
        }

        {
            int counter = 0;
            do {
                if (*cameraTime > 0.0f) {
                    Juice::GameHook::Instance()->LogText(
                        "[EVENT LOGGED] - CAMERA_MODE_TIME\n");
                    Juice::GameHook::Instance()->GameEvent(
                        "CAMERA_MODE_TIME", raceTypeString, "", counter, *cameraTime, "", "", "");
                }
                counter++;
                cameraTime++;
            } while (counter < 6);
        }

        {
            int counter = 5;
            do {
                if (*raceTime > 0.0f) {
                    Juice::GameHook::Instance()->LogText(
                        "[EVENT LOGGED] - RACE_TYPE_TIME\n");
                    Juice::GameHook::Instance()->GameEvent(
                        "RACE_TYPE_TIME", raceTypeString, "", 0, *raceTime, "", "", "");
                }
                counter--;
                raceTime++;
            } while (counter > -1);
        }
        ResetPerRaceStats();
    }
}

void JuiceStatsDB::CompareOptions(OptionsSettings *compare) {
    char tempStr[128];
    char cameraString[32];
    if (compare->TheAudioSettings.AmbientVol != mCurrentOptions.TheAudioSettings.AmbientVol) {
        bSPrintf(tempStr, "Ambient Volume - %f", compare->TheAudioSettings.AmbientVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.AudioMode != mCurrentOptions.TheAudioSettings.AudioMode) {
        bSPrintf(tempStr, "Audio Mode - %d", compare->TheAudioSettings.AudioMode);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.CarVol != mCurrentOptions.TheAudioSettings.CarVol) {
        bSPrintf(tempStr, "Car Volume - %f", compare->TheAudioSettings.CarVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.EngineVol != mCurrentOptions.TheAudioSettings.EngineVol) {
        bSPrintf(tempStr, "Engine Volume - %f", compare->TheAudioSettings.EngineVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.FEMusicVol != mCurrentOptions.TheAudioSettings.FEMusicVol) {
        bSPrintf(tempStr, "Music Volume - %f", compare->TheAudioSettings.FEMusicVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.MasterVol != mCurrentOptions.TheAudioSettings.MasterVol) {
        bSPrintf(tempStr, "Master Volume - %f", compare->TheAudioSettings.MasterVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.SoundEffectsVol != mCurrentOptions.TheAudioSettings.SoundEffectsVol) {
        bSPrintf(tempStr, "Sound Effects Volume - %f", compare->TheAudioSettings.SoundEffectsVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.SpeechVol != mCurrentOptions.TheAudioSettings.SpeechVol) {
        bSPrintf(tempStr, "Speech Volume - %d", compare->TheAudioSettings.SpeechVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheAudioSettings.SpeedVol != mCurrentOptions.TheAudioSettings.SpeedVol) {
        bSPrintf(tempStr, "Speed Volume - %d", compare->TheAudioSettings.SpeedVol);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheVideoSettings.FEScale != mCurrentOptions.TheVideoSettings.FEScale) {
        bSPrintf(tempStr, "FE Scale - %f", compare->TheVideoSettings.FEScale);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheGameplaySettings.Damage != mCurrentOptions.TheGameplaySettings.Damage) {
        bSPrintf(tempStr, "Damage - %d", compare->TheGameplaySettings.Damage);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheGameplaySettings.HighlightCam != mCurrentOptions.TheGameplaySettings.HighlightCam) {
        bSPrintf(tempStr, "Highlight Camera - %f", compare->TheGameplaySettings.HighlightCam);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheGameplaySettings.JumpCam != mCurrentOptions.TheGameplaySettings.JumpCam) {
        bSPrintf(tempStr, "Jump Camera - %d", compare->TheGameplaySettings.JumpCam);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->TheGameplaySettings.RearviewOn != mCurrentOptions.TheGameplaySettings.RearviewOn) {
        bSPrintf(tempStr, "Rear View - %d", compare->TheGameplaySettings.RearviewOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].LapInfoOn != mCurrentOptions.ThePlayerSettings[0].LapInfoOn) {
        bSPrintf(tempStr, "Lap Info - %d", compare->ThePlayerSettings[0].LapInfoOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }

    ePlayerSettingsCameras camera = compare->ThePlayerSettings[0].CurCam;
    if (camera != mCurrentOptions.ThePlayerSettings[0].CurCam) {
        switch (camera) {
        case PSC_BUMPER:
            bStrCpy(cameraString, "Bumper");
            break;
        case PSC_HOOD:
            bStrCpy(cameraString, "Hood");
            break;
        case PSC_CLOSE:
            bStrCpy(cameraString, "Close");
            break;
        case PSC_FAR:
            bStrCpy(cameraString, "Far");
            break;
        case PSC_SUPER_FAR:
            bStrCpy(cameraString, "Super Far");
            break;
        case PSC_DRIFT:
            bStrCpy(cameraString, "Drift");
            break;
        case PSC_PURSUIT:
            bStrCpy(cameraString, "Pursuit");
            goto camera_string_ready;
        default:
            goto camera_string_ready;
        }
camera_string_ready:
        bSPrintf(tempStr, "Current Camera - %s", cameraString);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].GaugesOn != mCurrentOptions.ThePlayerSettings[0].GaugesOn) {
        bSPrintf(tempStr, "Gauges - %d", compare->ThePlayerSettings[0].GaugesOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].Config != mCurrentOptions.ThePlayerSettings[0].Config) {
        bSPrintf(tempStr, "Controller Config - %d", compare->ThePlayerSettings[0].Config + 1);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].LapInfoOn != mCurrentOptions.ThePlayerSettings[0].LapInfoOn) {
        bSPrintf(tempStr, "Lap Info - %d", compare->ThePlayerSettings[0].LapInfoOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].LeaderboardOn != mCurrentOptions.ThePlayerSettings[0].LeaderboardOn) {
        bSPrintf(tempStr, "Leaderboard - %d", compare->ThePlayerSettings[0].LeaderboardOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].PositionOn != mCurrentOptions.ThePlayerSettings[0].PositionOn) {
        bSPrintf(tempStr, "Position - %d", compare->ThePlayerSettings[0].PositionOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].Rumble != mCurrentOptions.ThePlayerSettings[0].Rumble) {
        bSPrintf(tempStr, "Rumble - %d", compare->ThePlayerSettings[0].Rumble);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
    if (compare->ThePlayerSettings[0].ScoreOn != mCurrentOptions.ThePlayerSettings[0].ScoreOn) {
        bSPrintf(tempStr, "Score - %d", compare->ThePlayerSettings[0].ScoreOn);
        reinterpret_cast<Juice::GameHook *(*)()>(Juice::GameHook::Instance)()->AssetHit("OPTIONS", tempStr);
    }
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
