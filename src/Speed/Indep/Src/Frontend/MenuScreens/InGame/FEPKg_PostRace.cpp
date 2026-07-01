#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MEnterSafeHouse.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceAbandoned.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GTimer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

PursuitData PostRacePursuitScreen::mPursuitData;

RaceStat::RaceStat(FEString *title, FEString *data) : FEStatWidget(true) {
    SetTitleObject(title);
    SetDataObject(data);
}

StatsPanel::StatsPanel() : TheStats() {
    iWidgetToAdd = 1;
    RacerName = "";
    ParentPkg = "";
}

void StatsPanel::Reset() {
    TheStats.DeleteAllElements();
    iWidgetToAdd = 1;
}

void StatsPanel::Draw(unsigned int numPlayers) {
    if (numPlayers > 1 && RacerName != nullptr && bStrCmp(RacerName, "") != 0) {
        if (!FEngIsScriptSet(ParentPkg, 0x8A41F5B9, 0x5079C8F8)) {
            FEngSetScript(ParentPkg, 0x8A41F5B9, 0x5079C8F8, true);
        }

        FEngSetButtonTexture(FEngFindImage(ParentPkg, 0x5BC), 0x5BC);
        FEngSetButtonTexture(FEngFindImage(ParentPkg, 0x682), 0x682);
        FEPrintf(ParentPkg, 0xEB43CCB0, "%s", RacerName);
    } else if (!FEngIsScriptSet(ParentPkg, 0x8A41F5B9, 0x0016A259)) {
        FEngSetScript(ParentPkg, 0x8A41F5B9, 0x0016A259, true);
    }

    {
        FEWidget *widgey = TheStats.GetHead();
        while (widgey != TheStats.EndOfList()) {
            widgey->Draw();
            widgey = widgey->GetNext();
        }
    }
}

void StatsPanel::AddStat(RaceStat *stat) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    TheStats.AddTail(stat);
    ++iWidgetToAdd;
}

void StatsPanel::AddInfoStat(unsigned int title, unsigned int info) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    InfoStat *stat = new ("", 0) InfoStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), title, info);
    TheStats.AddTail(stat);
    ++iWidgetToAdd;
}

void StatsPanel::AddGenericStat(float stat_data, unsigned int title_hash, unsigned int units_hash, const char *format) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    GenericStat *stat =
        new ("", 0) GenericStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), stat_data, title_hash, units_hash, format);
    TheStats.AddTail(stat);
    ++iWidgetToAdd;
}

void StatsPanel::AddTimerStat(float seconds, unsigned int title_hash) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    TimerStat *stat = new ("", 0) TimerStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), seconds, title_hash);
    TheStats.AddTail(stat);
    ++iWidgetToAdd;
}

PostRaceResultsScreen::PostRaceResultsScreen(ScreenConstructorData *sd) : MenuScreen(sd), RacerStats(), RaceResults() {
    mNumberOfRacers = GRaceStatus::Get().GetRacerCount();
    mIndexOfWinner = -1;
    mIndexOfCurrentRacer = -1;
    mNumberOfLaps = GRaceStatus::Get().GetRaceParameters()->GetNumLaps();
    mNumberOfStats = 0;
    mRaceType = GRaceStatus::Get().GetRaceType();
    mPostRaceScreenMode = POSTRACESCREENMODE_RESULTS;
    mPlayerRacerInfo = nullptr;
    mMaxSlotsLeftSide = 11;
    m_RaceButtonHash = 0x5CED1D04;
    m_raceResultsUploaded = false;
    bEnableEAMessenger = false;

    if (mRaceType == GRace::kRaceType_Tollbooth) {
        mPostRaceScreenMode = POSTRACESCREENMODE_LAPSTATS;
    }

    for (int i = 0; i < mNumberOfRacers; ++i) {
        GRacerInfo *info = &GRaceStatus::Get().GetRacerInfo(i);

        if (info->GetSimable() != nullptr && mIndexOfCurrentRacer == -1 && info->GetSimable()->IsPlayer()) {
            mPlayerRacerInfo = info;
            mIndexOfCurrentRacer = i;
            break;
        }
    }

    for (int i = 0; i < 16; ++i) {
        RacerStats[i].SetParentPkg(GetPackageName());
    }
    RaceResults.SetParentPkg(GetPackageName());

    Setup();
}

PostRaceResultsScreen::~PostRaceResultsScreen() {}

void PostRaceResultsScreen::Setup() {
    for (int i = 0; i < mNumberOfRacers; ++i) {
        GRacerInfo *info = &GRaceStatus::Get().GetRacerInfo(i);

        if (info->IsFinishedRacing() && info->GetRanking() == 1) {
            mIndexOfWinner = i;
            break;
        }
    }

    for (int i = 0; i < mMaxSlotsLeftSide;) {
        int slot = i + 1;
        FEngSetScript(GetPackageName(), FEngHashString("LINE%d_GROUP"), 0x0016A259, true);

        if (mPostRaceScreenMode == POSTRACESCREENMODE_STATS ||
            (mPostRaceScreenMode == POSTRACESCREENMODE_LAPSTATS && mRaceType == GRace::kRaceType_Tollbooth)) {
            FEngSetInvisible(FEngFindObject(GetPackageName(), FEngHashString("COLUMN2_DATA%d", slot)));
        } else {
            FEngSetVisible(FEngFindObject(GetPackageName(), FEngHashString("COLUMN2_DATA%d", slot)));
        }

        i = slot;
    }

    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x586AB4A6));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x44AC8987));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x30EE5E68));

    switch (mPostRaceScreenMode) {
        case POSTRACESCREENMODE_RESULTS:
            RaceResults.Reset();
            SetupResults();
            RaceResults.Draw(mNumberOfRacers);
            break;
        case POSTRACESCREENMODE_STATS:
            RacerStats[mIndexOfCurrentRacer].Reset();
            SetupRacerStats(mIndexOfCurrentRacer, &GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer));
            RacerStats[mIndexOfCurrentRacer].Draw(mNumberOfRacers);
            break;
        case POSTRACESCREENMODE_LAPSTATS:
            RacerStats[mIndexOfCurrentRacer].Reset();
            SetupLapStats(mIndexOfCurrentRacer, &GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer));
            RacerStats[mIndexOfCurrentRacer].Draw(mNumberOfRacers);
            break;
        default:
            break;
    }

    unsigned int fe_flags = FEDatabase->GetGameMode();

    if ((fe_flags & 8) == 0) {
        if ((fe_flags & 0x40) == 0 && !FEngIsScriptSet(GetPackageName(), 0x445A862B, 0x5079C8F8)) {
            FEngSetScript(GetPackageName(), 0x445A862B, 0x5079C8F8, true);
        }
    }
}

void PostRaceResultsScreen::SetupResults() {
    FEngSetVisible(GetPackageName(), 0x586AB4A6);
    FEngSetVisible(GetPackageName(), 0x44AC8987);
    FEngSetVisible(GetPackageName(), 0x30EE5E68);

    if (mRaceType >= GRace::kRaceType_P2P) {
        if (mRaceType < GRace::kRaceType_Tollbooth) {
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
        } else if (mRaceType == GRace::kRaceType_SpeedTrap) {
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0x7540FB04);
        }
    }

    FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xFF115FFF);
    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xD0B8AA33);

    unsigned int speed_units = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speed_units = 0x8569A25F;
    }

    mNumberOfStats = 0;
    RaceResults.Reset();

    if (mRaceType >= GRace::kRaceType_P2P) {
        if (mRaceType <= GRace::kRaceType_Tollbooth) {
            int place = 0;
            if (place < mNumberOfRacers) {
                do {
                    ++place;
                    int i = 0;
                    GRacerInfo *racer_info = nullptr;

                    while (true) {
                        racer_info = &GRaceStatus::Get().GetRacerInfo(i);
                        if (racer_info->GetRanking() == place) {
                            break;
                        }
                        ++i;
                    }

                    RaceResults.AddStat(new ("", 0)
                                            RaceResultStat(RaceResults.GetCurrentString("COLUMN2_DATA"), RaceResults.GetCurrentString("COLUMN3_DATA"),
                                                           RaceResults.GetCurrentString("COLUMN1_DATA"), racer_info));
                } while (place < mNumberOfRacers);
            }
        } else if (mRaceType == GRace::kRaceType_SpeedTrap) {
            int place = 0;
            if (place < mNumberOfRacers) {
                do {
                    ++place;
                    int i = 0;
                    GRacerInfo *racer_info = nullptr;

                    while (true) {
                        racer_info = &GRaceStatus::Get().GetRacerInfo(i);
                        if (racer_info->GetRanking() == place) {
                            break;
                        }
                        ++i;
                    }

                    float speed = racer_info->GetPointTotal();
                    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
                        speed = (speed * 0.27778f) * 0.23699f;
                    }

                    RaceResults.AddStat(new ("", 0)
                                            GenericResult(RaceResults.GetCurrentString("COLUMN2_DATA"), RaceResults.GetCurrentString("COLUMN3_DATA"),
                                                          RaceResults.GetCurrentString("COLUMN1_DATA"), speed_units, speed, "%$0.0f", racer_info));
                } while (place < mNumberOfRacers);
            }
        }
    }
}

void PostRaceResultsScreen::SetupStat_NosUsed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int mass_units = 0xC173E1BB;
    float nos = *reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x11C);

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        mass_units = 0xC173DE1F;
        nos *= 0.45359f;
    }

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(nos, 0x114E759F, mass_units, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x114E759F, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TopSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569AB44;
    float speed = *reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x114) * 2.23699f;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed *= 1.60931f;
    }

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x0EF34382, speedUnits, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x0EF34382, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AverageSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569AB44;
    float speed = racerInfo.CalcAverageSpeed() * 2.23669f;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed *= 1.60931f;
    }

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x57F4140A, speedUnits, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x57F4140A, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TimeBehind() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (mIndexOfWinner >= 0 && mIndexOfWinner != mIndexOfCurrentRacer) {
        GRacerInfo &winnerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfWinner);
        float winnerTime = reinterpret_cast<const GTimer *>(reinterpret_cast<const char *>(&winnerInfo) + 0x160)->GetTime();
        float racerTime = reinterpret_cast<const GTimer *>(reinterpret_cast<const char *>(&racerInfo) + 0x160)->GetTime();

        RacerStats[mIndexOfCurrentRacer].AddTimerStat(bAbs(winnerTime - racerTime), 0xAB44ED8B);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0xAB44ED8B, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_LapVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (*reinterpret_cast<const int *>(reinterpret_cast<const char *>(&racerInfo) + 0x3C) > 1) {
        float bestLapTime = GRaceStatus::Get().GetBestLapTime(mIndexOfCurrentRacer);
        float worstLapTime = GRaceStatus::Get().GetWorstLapTime(mIndexOfCurrentRacer);

        RacerStats[mIndexOfCurrentRacer].AddTimerStat(worstLapTime - bestLapTime, 0x4121E8C4);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x4121E8C4, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_StageVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
}

void PostRaceResultsScreen::SetupStat_TrafficCollisions() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(
            static_cast<float>(*reinterpret_cast<const int *>(reinterpret_cast<const char *>(&racerInfo) + 0x12C)), 0x094BFDFC, 0, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x094BFDFC, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_ZeroToSixty() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0xCCBC22B3;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0xB8CF16FC;
    }

    if (*reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x138) > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(*reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x138),
                                                      speedUnits);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(speedUnits, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_QuarterMile() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int timeUnits = 0x49FD5DCB;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        timeUnits = 0x1C6F2A82;
    }

    if (*reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x13C) > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(*reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x13C),
                                                      timeUnits);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(timeUnits, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_PerfectShifts() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(
            static_cast<float>(*reinterpret_cast<const int *>(reinterpret_cast<const char *>(&racerInfo) + 0x128)), 0x680AC597, 0, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x680AC597, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AccumulatedSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569A25F;
    float speed = *reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x134);

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
        speedUnits = 0x8569AB44;
        speed = (speed * 0.27778f) * 2.23699f;
    }

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0xD57E02AB, speedUnits, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0xD57E02AB, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_SpeedVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    float bestSpeed = GRaceStatus::Get().GetBestSpeedTrapSpeed(mIndexOfCurrentRacer);
    float worstSpeed = GRaceStatus::Get().GetWorstSpeedTrapSpeed(mIndexOfCurrentRacer);

    if (bestSpeed > 0.0f && worstSpeed > 0.0f) {
        unsigned int speed_units = 0x8569A25F;
        float speed = worstSpeed - bestSpeed;

        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
            speed_units = 0x8569AB44;
            speed = MPS2MPH(KPH2MPS(speed));
        }

        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x6EEABE8C, speed_units, "%.2f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x6EEABE8C, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_SpeedBehind() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (mIndexOfWinner >= 0) {
        unsigned int speed_units = 0x8569A25F;
        GRacerInfo &winnerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfWinner);
        float speed = bAbs(winnerInfo.GetPointTotal() - racerInfo.GetPointTotal());

        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
            speed_units = 0x8569AB44;
            speed = (speed * 0.27778f) * 2.23699f;
        }

        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x2E54B7ED, speed_units, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x2E54B7ED, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupRacerStats(int index, GRacerInfo *racer_info) {
    FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x4E706980);
    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Drag:
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x1135F776);
            break;
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout:
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x7B8F45DF);
            break;
        case GRace::kRaceType_SpeedTrap:
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xAEF51E9D);
            break;
        case GRace::kRaceType_Tollbooth:
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xAC23368C);
            break;
        default:
            break;
    }

    RacerStats[index].SetRacerName(racer_info->GetName());

    switch (mRaceType) {
        case GRace::kRaceType_P2P:
            SetupStat_TopSpeed();
            SetupStat_AverageSpeed();
            if (FEDatabase->IsLANMode()) {
                SetupStat_NosUsed();
            } else if (FEDatabase->IsOnlineMode()) {
                SetupStat_NosUsed();
            } else {
                SetupStat_TimeBehind();
                SetupStat_TrafficCollisions();
            }
            SetupStat_StageVariance();
            break;
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout:
            SetupStat_TopSpeed();
            SetupStat_AverageSpeed();
            if (FEDatabase->IsLANMode()) {
                SetupStat_NosUsed();
            } else if (FEDatabase->IsOnlineMode()) {
                SetupStat_NosUsed();
            } else {
                SetupStat_TimeBehind();
                SetupStat_LapVariance();
                SetupStat_TrafficCollisions();
            }
            break;
        case GRace::kRaceType_Drag:
            SetupStat_ZeroToSixty();
            SetupStat_QuarterMile();
            SetupStat_PerfectShifts();
            if (FEDatabase->IsLANMode()) {
                SetupStat_NosUsed();
            } else if (FEDatabase->IsOnlineMode()) {
                SetupStat_NosUsed();
            } else {
                SetupStat_TimeBehind();
                SetupStat_TrafficCollisions();
            }
            break;
        case GRace::kRaceType_Tollbooth:
            SetupStat_AccumulatedSpeed();
            SetupStat_SpeedVariance();
            SetupStat_SpeedBehind();
            break;
        case GRace::kRaceType_SpeedTrap:
            SetupStat_TopSpeed();
            SetupStat_AverageSpeed();
            SetupStat_StageVariance();
            SetupStat_TrafficCollisions();
            break;
        default:
            break;
    }
}

void PostRaceResultsScreen::SetupLapStats(int racerIndex, GRacerInfo *racer_info) {
    FEObject *obj = nullptr;

    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x8159A0B2);
    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Drag:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x34BA50FF);
            obj = FEngFindObject(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x44AC8987);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x30EE5E68);
            FEngSetVisible(obj);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xE8B7D527);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            break;
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x9C8D7FE8);
            obj = FEngFindObject(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x44AC8987);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x30EE5E68);
            FEngSetVisible(obj);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x0000BF9C);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            break;
        case GRace::kRaceType_SpeedTrap:
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().SortCheckPointRankings();
            }
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xECD0E6A6);
            obj = FEngFindObject(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x44AC8987);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x30EE5E68);
            FEngSetVisible(obj);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xEE1EDC76);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0x7540FB04);
            break;
        case GRace::kRaceType_Tollbooth:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xD10A8EA2);
            obj = FEngFindObject(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(obj);
            obj = FEngFindObject(GetPackageName(), 0x30EE5E68);
            FEngSetVisible(obj);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xA15E4505);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xD0B8AA33);
            break;
        default:
            break;
    }

    StatsPanel &panel = RacerStats[racerIndex];
    panel.SetRacerName(racer_info->GetName());
    GRaceStatus &race_status = GRaceStatus::Get();

    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Drag: {
            for (int i = 0; i < 4; ++i) {
                RacerStats[racerIndex].AddStat(new ("", 0) StageStat(
                    RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                    RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), i, racer_info->GetSplitTime(i), racer_info->GetSplitRanking(i)));
            }

            float final_time = 0.0f;
            if (racer_info->IsFinishedRacing()) {
                final_time = racer_info->GetRaceTime();
            }

            panel.AddStat(new ("", 0) StageStat(panel.GetCurrentString("COLUMN1_DATA"), panel.GetCurrentString("COLUMN3_DATA"),
                                                panel.GetCurrentString("COLUMN2_DATA"), 4, final_time, racer_info->GetRanking()));
            break;
        }
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout: {
            for (int i = 0; i < race_status.GetRaceParameters()->GetNumLaps(); ++i) {
                int lap_position = race_status.GetLapPosition(i, racerIndex, true);

                if (racer_info->GetIsKnockedOut() && lap_position < 2) {
                    lap_position = -1;
                }

                RacerStats[racerIndex].AddStat(new ("", 0) LapStat(
                    RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                    RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), i + 1, race_status.GetLapTime(i, racerIndex, false), lap_position));
            }
            break;
        }
        case GRace::kRaceType_SpeedTrap: {
            int num_traps = race_status.GetNumRaceSpeedTraps();

            for (int i = 0; i < num_traps; ++i) {
                panel.AddStat(new ("", 0) SpeedStat(panel.GetCurrentString("COLUMN1_DATA"), panel.GetCurrentString("COLUMN3_DATA"),
                                                    panel.GetCurrentString("COLUMN2_DATA"), i + 1, race_status.GetRaceSpeedTrapSpeed(i, racerIndex),
                                                    race_status.GetRaceSpeedTrapPosition(i, racerIndex)));
            }
            break;
        }
        case GRace::kRaceType_Tollbooth: {
            int num_booths = race_status.GetNumRaceTollbooths();

            for (int i = 0; i < num_booths; ++i) {
                RacerStats[racerIndex].AddStat(new ("", 0) TollboothStat(
                    RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                    RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), i + 1, race_status.GetRaceTollboothTime(i, racerIndex), 1));
            }

            float remaining_time = 0.0f;
            if (racer_info->IsFinishedRacing()) {
                remaining_time = race_status.GetRaceTimeRemaining();
            }

            panel.AddStat(new ("", 0) TollboothStat(panel.GetCurrentString("COLUMN1_DATA"), panel.GetCurrentString("COLUMN3_DATA"),
                                                    panel.GetCurrentString("COLUMN2_DATA"), num_booths + 1, remaining_time, 1));
            break;
        }
        default:
            break;
    }
}

void PostRaceResultsScreen::NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) {
    switch (msg) {
        case 0x35F8620B: {
            if (!FEDatabase->IsLANMode()) {
                if (!FEDatabase->IsOnlineMode()) {
                    return;
                }
            }

            FEngSetScript(GetPackageName(), 0x812A09D4, 0x0016A259, true);
            FEngSetScript(GetPackageName(), 0x05D85A9F, 0x5079C8F8, true);
            return;
        }
        case 0x5073EF13:
            if (mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
                return;
            }

            --mIndexOfCurrentRacer;
            if (mIndexOfCurrentRacer < 0) {
                mIndexOfCurrentRacer = mNumberOfRacers - 1;
            }

            Setup();
            return;
        case 0xD9FEEC59:
            if (mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
                return;
            }

            ++mIndexOfCurrentRacer;
            if (mIndexOfCurrentRacer >= mNumberOfRacers) {
                mIndexOfCurrentRacer = 0;
            }

            Setup();
            return;
        case 0xC519BFC3:
            if (mRaceType == GRace::kRaceType_Tollbooth) {
                if (mPostRaceScreenMode == POSTRACESCREENMODE_LAPSTATS) {
                    mPostRaceScreenMode = POSTRACESCREENMODE_STATS;
                } else {
                    mPostRaceScreenMode = POSTRACESCREENMODE_LAPSTATS;
                }
            } else {
                mPostRaceScreenMode = static_cast<PostRaceScreenMode>(mPostRaceScreenMode + 1);
                if (mPostRaceScreenMode == POSTRACESCREENMODE_NUMMODES) {
                    mPostRaceScreenMode = POSTRACESCREENMODE_RESULTS;
                }
            }

            Setup();
            return;
        case 0xC519BFC4: {
            if (FEDatabase->IsLANMode()) {
                return;
            }

            if (FEDatabase->IsOnlineMode()) {
                return;
            }

            DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, 0x417B2601, 0x1A294DAD, 0xE1A57D51, 0xB4623F67,
                                            0xB4623F67, first_dialog_button2, static_cast<unsigned int>(0x4D3399A8));
            return;
        }
        case 0xE1A57D51:
            new EUnPause();
            if (cFEng::Get()->IsPackagePushed("InGamePhotoMaster.fng")) {
                PhotoFinishScreen::mRestartSelected = true;
                return;
            }

            new ERestartRace();
            return;
        case 0xB4623F67:
            cFEng::Get()->QueuePackageMessage(0xC6341FF6, GetPackageName(), nullptr);
            return;
        case 0x406415E3: {
            if (FEngIsScriptSet(GetPackageName(), 0x57EFB2FB, 0x0016A259)) {
                return;
            }

            if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career && GRaceStatus::Get().GetRaceParameters()->GetIsBossRace()) {
                bool playerDone = false;

                if (mPlayerRacerInfo != nullptr &&
                    (mPlayerRacerInfo->IsFinishedRacing() || mPlayerRacerInfo->GetIsTotalled() || mPlayerRacerInfo->GetIsEngineBlown() ||
                     mPlayerRacerInfo->GetIsKnockedOut() || mPlayerRacerInfo->GetIsBusted())) {
                    playerDone = true;
                }

                if (playerDone && mPlayerRacerInfo->GetRanking() != 1) {
                    DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, 0x417B2601, 0x1A294DAD, 0x30ED2368,
                                                    0xB4623F67, 0xB4623F67, first_dialog_button2, static_cast<unsigned int>(0x9887EB98));
                    return;
                }
            }

            goto set_continue_script;
        }
        case 0x30ED2368:
        set_continue_script:
            if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x001335F0)) {
                FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x001335F0, true);
            }
            return;
        case 0xE1FDE1D1: {
            if (FEDatabase->IsLANMode()) {
                return;
            }

            if (FEDatabase->IsOnlineMode()) {
                return;
            }

            if (cFEng::Get()->IsPackagePushed("InGamePhotoMaster.fng")) {
                cFEng::Get()->QueuePackagePop(1);
                if (cFEng::Get()->IsPackagePushed("InGameBackground.fng")) {
                    cFEng::Get()->QueuePackagePop(1);
                }
                return;
            }

            {
                bool playerDone = false;
                if (mPlayerRacerInfo != nullptr &&
                    (mPlayerRacerInfo->IsFinishedRacing() || mPlayerRacerInfo->GetIsTotalled() || mPlayerRacerInfo->GetIsEngineBlown() ||
                     mPlayerRacerInfo->GetIsKnockedOut() || mPlayerRacerInfo->GetIsBusted())) {
                    playerDone = true;
                }

                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                bool ddayRace = false;
                if (parms != nullptr && parms->GetIsDDayRace()) {
                    ddayRace = true;
                }

                if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career && !ddayRace) {
                    if (playerDone) {
                        GRaceStatus::Get().RaceAbandoned();

                        {
                            MNotifyRaceAbandoned abandoned;
                            abandoned.Post(MNotifyRaceAbandoned::_GetKind());
                        }
                    }

                    new EUnPause();
                    return;
                }

                if (playerDone) {
                    new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
                } else {
                    new EUnPause();
                }
            }
            return;
        }
        default:
            return;
    }
}

eMenuSoundTriggers PostRaceResultsScreen::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg != 0x7B6B89D7) {
        if (msg < 0x7B6B89D8) {
            if (msg != 0x4A805994) {
                return maybe;
            }
        } else if (msg != 0x9AFA53A7) {
            return maybe;
        }

        if (mNumberOfRacers < 2 || mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
            return static_cast<eMenuSoundTriggers>(-1);
        }
    }

    if (FEngIsScriptSet(GetPackageName(), 0x57EFB2FB, 0x0016A259)) {
        return static_cast<eMenuSoundTriggers>(-1);
    }

    return maybe;
}

// Range: 0x80155F40 -> 0x801560EC
void PursuitData::PopulateData(IPursuit *ipursuit, IPerpetrator *iperpetrator, int exitToSafehouse) {
    mPursuitIsActive = ipursuit && ipursuit->IsPursuitBailed() == false;

    if (ipursuit) {
        mPursuitLength = ipursuit->GetPursuitDuration();
        mNumCopsDamaged = ipursuit->GetNumCopsDamaged();
        mNumCopsDestroyed = ipursuit->GetNumCopsDestroyed();
        mNumSpikeStripsDodged = ipursuit->GetNumSpikeStripsDodged();
        mNumRoadblocksDodged = ipursuit->GetNumRoadblocksDodged();
        mCostToStateAchieved = ipursuit->CalcTotalCostToState();
    }

    if (iperpetrator) {
        if (iperpetrator->GetPendingRepPointsNormal() > 0) {
            mRepAchievedNormal = iperpetrator->GetPendingRepPointsNormal();
        }
        if (iperpetrator->GetPendingRepPointsFromCopDestruction() > 0) {
            mRepAchievedCopDestruction = iperpetrator->GetPendingRepPointsFromCopDestruction();
        }
    }

    if (exitToSafehouse >= 0) {
        mExitToSafehouse = exitToSafehouse;
    }
}

bool PursuitData::AddMilestone(GMilestone *milestone) {
    if (mNumMilestonesThisPursuit > 0x1f) {
        return false;
    }
    mMilestonesCompleted[mNumMilestonesThisPursuit] = milestone;
    mNumMilestonesThisPursuit = mNumMilestonesThisPursuit + 1;
    return true;
}

const GMilestone *const PursuitData::GetMilestone(int index) const {
    if (index > 0x1F) {
        return 0;
    }
    return mMilestonesCompleted[index];
}

void PursuitData::ClearData() {
    mPursuitIsActive = false;
    mPursuitLength = 0.0f;
    mNumCopsDamaged = 0;
    mNumCopsDestroyed = 0;
    mNumSpikeStripsDodged = 0;
    mNumRoadblocksDodged = 0;
    mCostToStateAchieved = 0;
    mRepAchievedNormal = 0;
    mRepAchievedCopDestruction = 0;
    mNumMilestonesThisPursuit = 0;
    for (int i = 0; i <= 0x1F; i++) {
        mMilestonesCompleted[i] = nullptr;
    }
}

PursuitResultsDatum::PursuitResultsDatum(PursuitResultsDatumType type, uint32 itemName, float itemNumber, float itemGoal,
                                         PursuitResultsDatumCheckType itemChecked)
    : ArrayDatum(0, 0) {
    mType = type;
    mName = itemName;
    mNumber = itemNumber >= 0.0f ? itemNumber : 0.0f;
    mGoal = itemGoal;
    mChecked = itemChecked;
}

void PursuitResultsDatum::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {}

PursuitResultsArraySlot::PursuitResultsArraySlot(FEObject *obj, FEString *itemName, FEString *itemNumber, FEImage *itemChecked, FEImage *itemEmpty)
    : ArraySlot(obj) {
    mLine = obj;
    mItemName = itemName;
    mItemNumber = itemNumber;
    mItemChecked = itemChecked;
    mItemEmpty = itemEmpty;
}

void PursuitResultsArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (!datum) {
        return;
    }

    PursuitResultsDatum *data = static_cast<PursuitResultsDatum *>(datum);

    FEngSetScript(mItemChecked, 0x16A259, true);
    FEngSetScript(mItemEmpty, 0x16A259, true);
    FEngSetScript(mLine, 0x1744B3, true);

    if (mItemName) {
        FEngSetScript(mItemName, 0x1744B3, true);
        FEngSetLanguageHash(mItemName, data->GetName());
    }

    PursuitResultsDatum::PursuitResultsDatumType type = data->GetType();

    if (type == PursuitResultsDatum::PursuitResultsDatumType_Number) {
        FEPrintf(mItemNumber, "%d", data->GetNumber());
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->GetChecked()) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->GetGreyed()) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Time) {
        Timer timer;
        timer.SetTime(data->GetNumber());
        char text[32];
        timer.PrintToString(text, 0);
        FEPrintf(mItemNumber, "%s", text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->IsChecked()) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->GetGreyed()) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number) {
        char text[32];
        FEDatabase->SetMilestoneDescriptionString(text, -1, data->GetNumber(), data->GetGoal(), false);
        FEPrintf(mItemNumber, "%s", text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->IsChecked()) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->GetGreyed()) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time) {
        char text[32];
        FEDatabase->SetMilestoneDescriptionString(text, -1, data->GetNumber(), data->GetGoal(), true);
        FEPrintf(mItemNumber, "%s", text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->IsChecked()) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->GetGreyed()) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Time) {
        FEngSetScript(mItemNumber, 0x16A259, true);
        if (data->IsChecked()) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x163C76, true);
        }
    }
}

PostRacePursuitScreen::PostRacePursuitScreen(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 1, 9, false) {
    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_PURSUIT;
    m_RaceButtonHash = 0x5CED1D04;

    int i;
    char sztemp[32];

    i = 0;
    while (i < GetHeight()) {
        i++;
        FEngSNPrintf(sztemp, 0x20, "LINE%d_GROUP", i);
        FEObject *wrapperGroup = FEngFindObject(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 0x20, "COLUMN1_DATA%d", i);
        FEString *itemName = FEngFindString(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 0x20, "COLUMN2_DATA%d", i);
        FEString *itemValue = FEngFindString(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 0x20, "CHECK%d", i);
        FEImage *checkMark = FEngFindImage(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 0x20, "EMPTY%d", i);
        FEImage *emptyMark = FEngFindImage(GetPackageName(), FEHashUpper(sztemp));
        AddSlot(new ("", 0) PursuitResultsArraySlot(wrapperGroup, itemName, itemValue, checkMark, emptyMark));
    }
    Initialize();
}

PostRacePursuitScreen::~PostRacePursuitScreen() {
    if (GetPursuitData().mExitToSafehouse != 0) {
        GetPursuitData().mExitToSafehouse = 0;
        UCrc32 postCrc(0x20D60DBF);
        MEnterSafeHouse msg("safehouse");
        msg.Post(postCrc);
    }
}

void PostRacePursuitScreen::Initialize() {
    ClearData();
    if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_INFRACTIONS) {
        FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xFB415E78);
        if (TheGameFlowManager.IsInFrontend()) {
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x7448870B);
        } else {
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x74413352);
            if (GRaceStatus::Exists()) {
                GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
                if (raceParams && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
                    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x9145A5F2);
                }
            }
        }
        SetupInfractions();
    } else if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_MILESTONES) {
        FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x578B767B);
        if (GRaceStatus::Exists()) {
            GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
            if (raceParams && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
                FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x334FA7FB);
            }
        }
        FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x7448870B);
        SetupMilestones();
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xFEA872D4);
        FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xAEAEB62F);
        SetupPursuit();
    }
    SetInitialPosition(0);
    ArrayScroller::RefreshHeader();
}

void PostRacePursuitScreen::SetupInfractions() {
    PursuitResultsDatum::PursuitResultsDatumCheckType checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;

    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Racing)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x4b0ff103, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Speeding)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x8ec1a6ec, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Reckless)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x370f331d, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Assault)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x8462a784, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_HitAndRun)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0xdff254ba, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Damage)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x7dbd5b34, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Resist)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x2b1de2a9, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_OffRoad)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0xb0ef5c12, 0.0f, 0.0f, checkType));
}

void PostRacePursuitScreen::SetupMilestones() {
    if (GRaceStatus::Exists()) {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
        if (raceParams && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
            PursuitResultsDatum::PursuitResultsDatumType type = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number;
            if (FEDatabase->IsMilestoneTimeFormat(raceParams->GetChallengeType())) {
                type = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time;
            }
            float bestVal = GManager::Get().GetBestValue(raceParams->GetChallengeType());
            float goalVal = raceParams->GetChallengeGoal();
            if (raceParams->GetChallengeType() == 0x5392e4fd) {
                type = PursuitResultsDatum::PursuitResultsDatumType_Time;
            }
            PursuitResultsDatum::PursuitResultsDatumCheckType checkType =
                static_cast<PursuitResultsDatum::PursuitResultsDatumCheckType>(static_cast<int>(bestVal >= goalVal));
            AddDatum(
                new PursuitResultsDatum(type, FEDatabase->GetChallengeHeaderHash(raceParams->GetLocalizationTag()), bestVal, goalVal, checkType));
        } else {
            unsigned int binNumber = FEDatabase->GetCareerSettings()->GetCurrentBin();
            GMilestone *milestone = GManager::Get().GetFirstMilestone(false, binNumber);
            while (milestone) {
                PursuitResultsDatum::PursuitResultsDatumType type = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number;
                if (FEDatabase->IsMilestoneTimeFormat(milestone->GetTypeKey())) {
                    type = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time;
                }
                if (milestone->GetTypeKey() == 0x5392e4fd) {
                    type = PursuitResultsDatum::PursuitResultsDatumType_Time;
                }
                PursuitResultsDatum::PursuitResultsDatumCheckType checkType;
                if (milestone->GetIsDonePendingEscape()) {
                    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
                } else {
                    checkType = static_cast<PursuitResultsDatum::PursuitResultsDatumCheckType>(milestone->GetIsAwarded() ? 2 : 0);
                }
                AddDatum(new PursuitResultsDatum(type, FEDatabase->GetMilestoneHeaderHash(milestone->GetLocalizationTag()),
                                                 milestone->GetCurrentValue(), milestone->GetRequiredValue(), checkType));
                milestone = GManager::Get().GetNextMilestone(milestone, false, binNumber);
            }
        }
    }
}

void PostRacePursuitScreen::SetupPursuit() {
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Time, 0x4d64888d, mPursuitData.mPursuitLength,
                                                          0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xa999f6e2,
                                     static_cast<float>(mPursuitData.mNumCopsDamaged), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x23f6e732,
                                                          static_cast<float>(mPursuitData.mNumCopsDestroyed), 0.0f,
                                                          PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x0291c816,
                                                          static_cast<float>(mPursuitData.mNumSpikeStripsDodged), 0.0f,
                                                          PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x29daba15,
                                                          static_cast<float>(mPursuitData.mNumRoadblocksDodged), 0.0f,
                                                          PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xd9bb7d2d,
                                                          static_cast<float>(mPursuitData.mCostToStateAchieved), 0.0f,
                                                          PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new (__FILE__, __LINE__) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xb7dfff96,
                                                          static_cast<float>(GInfractionManager::Get().GetNumInfractions()), 0.0f,
                                                          PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
}

void PostRacePursuitScreen::NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pObject, Param1, Param2);
    switch (msg) {
        case 0x406415E3:
            if (TheGameFlowManager.IsInFrontend()) {
                if (FEDatabase->IsQuickRaceMode()) {
                    cFEng::Get()->QueuePackageSwitch("ChallengeSeries.fng", 0, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
                }
            } else if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
                GRacerInfo &info = GRaceStatus::Get().GetRacerInfo(0);
                if (info.IsFinishedRacing() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
                    if (FEDatabase->IsChallengeMode() && MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                        MemcardEnter(nullptr, nullptr, 0x100B1, nullptr, nullptr, 0, 0);
                    } else {
                        new EQuitToFE(static_cast<eGarageType>(1), nullptr);
                    }
                } else {
                    new EUnPause();
                }
            } else {
                new EUnPause();
            }
            break;
        case 0xC519BFC3:
            if (TheGameFlowManager.IsInFrontend()) {
                if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_INFRACTIONS) {
                    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_PURSUIT;
                } else {
                    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_INFRACTIONS;
                }
            } else {
                if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_INFRACTIONS) {
                    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_MILESTONES;
                } else if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_MILESTONES) {
                    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_PURSUIT;
                } else {
                    mPostPursuitScreenMode = POSTPURSUITSCREENMODE_INFRACTIONS;
                }
            }
            Initialize();
            break;
    }
}

PostRaceMilestonesScreen::PostRaceMilestonesScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    mBountyEarned = 0.0f;
    mCurrMilestoneIndex = -1;
    mCurrMilestoneScriptHash = 0;
    mCopDestructionBountyShown = false;
    mpDataBigIcon = FEngFindImage(GetPackageName(), 0x14564FB9);
}

PostRaceMilestonesScreen::~PostRaceMilestonesScreen() {}

void PostRaceMilestonesScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x35f8620b) {
        StartBountyAnimations(false);
        return;
    }

    if (msg <= 0x35f8620a) {
        if (msg != 0xd3c3de7) {
            return;
        }

        if (!mCopDestructionBountyShown) {
            mCopDestructionBountyShown = true;
            if (PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed > 0) {
                StartBountyAnimations(true);
                return;
            }
        }

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace() &&
            !FEDatabase->IsFinalEpicChase()) {
            StartChallengeAnimations();
        } else {
            StartMilestoneAnimations();
        }
        return;
    }

    if (msg == 0x406415e3) {
        cFEng::Get()->QueuePackagePop(1);
        new EShowResults(FERESULTTYPE_PURSUIT, false);
        return;
    }

    if (msg != 0xc98356ba) {
        return;
    }

    if (FEngIsScriptSet(mpDataBigIcon, 0x5079c8f8) && !FEngIsScriptRunning(mpDataBigIcon, 0x5079c8f8)) {
        FEngSetScript(mpDataBigIcon, mCurrMilestoneScriptHash, true);
    }
}

bool PostRaceMilestonesScreen::StartMilestoneAnimations() {
    mCurrMilestoneIndex++;
    const GMilestone *const milestone = PostRacePursuitScreen::GetPursuitData().GetMilestone(mCurrMilestoneIndex);
    if (milestone) {
        char descStr[32];
        char outputStr[64];
        FEDatabase->SetMilestoneDescriptionString(descStr, milestone->GetTypeKey(), milestone->GetCurrentValue(), milestone->GetRequiredValue(),
                                                  false);
        bSNPrintf(outputStr, 64, "%s: %s", GetTranslatedString(FEDatabase->GetMilestoneHeaderHash(milestone->GetLocalizationTag())), descStr);
        StartAnimations(true, milestone->GetTypeKey(), milestone->GetBounty(), outputStr);
        return true;
    }

    StartMilestoneDoneAnimations();
    return false;
}

bool PostRaceMilestonesScreen::StartChallengeAnimations() {
    mCurrMilestoneIndex++;
    if (mCurrMilestoneIndex < 1 && GRaceStatus::Exists()) {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
        if (raceParams && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
            float currVal = GManager::Get().GetBestValue(raceParams->GetChallengeType());
            float goalVal = raceParams->GetChallengeGoal();
            char descStr[32];
            char outputStr[64];
            FEDatabase->SetMilestoneDescriptionString(descStr, raceParams->GetChallengeType(), currVal, goalVal, false);
            bSNPrintf(outputStr, 64, "%s: %s", GetTranslatedString(FEDatabase->GetChallengeHeaderHash(raceParams->GetLocalizationTag())), descStr);
            StartAnimations(true, raceParams->GetChallengeType(), static_cast<float>(raceParams->GetReputation()), outputStr);
            return true;
        }
    }
    StartMilestoneDoneAnimations();
    return false;
}

bool PostRaceMilestonesScreen::StartBountyAnimations(bool copDestruction) {
    unsigned int key;
    float bountyEarned;
    char outputStr[64];
    if (!copDestruction) {
        key = 0x33fa23a;
        bountyEarned = static_cast<float>(PostRacePursuitScreen::GetPursuitData().mRepAchievedNormal);
        bSNPrintf(outputStr, 64, "%s", GetTranslatedString(0x4d64888d));
    } else {
        key = 0x4fc942ca;
        bountyEarned = static_cast<float>(PostRacePursuitScreen::GetPursuitData().mRepAchievedCopDestruction);
        bSNPrintf(outputStr, 64, "%s: %$d", GetTranslatedString(0x23f6e732), PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed);
    }
    StartAnimations(false, key, bountyEarned, outputStr);
    return true;
}

void PostRaceMilestonesScreen::StartAnimations(bool isMilestone, int typeKey, float bountyEarned, const char *const descriptionStr) {
    mBountyEarned += bountyEarned;
    SetMilestoneAnimationScriptHash(isMilestone, typeKey);
    FEngSetTextureHash(mpDataBigIcon, FEDatabase->GetMilestoneIconHash(typeKey, isMilestone));
    FEPrintf(GetPackageName(), 0xe526d0d2, "%s", descriptionStr);
    if (bountyEarned > 0.0f) {
        FEngSetVisible(GetPackageName(), 0xe1045a4f);
    } else {
        FEngSetInvisible(GetPackageName(), 0xe1045a4f);
    }
    FEPrintf(GetPackageName(), 0xe1045a4f, "%s: %$0.0f", GetTranslatedString(0x29b1b96a), bountyEarned);
    FEPrintf(GetPackageName(), 0x324f7792, "%s: %$0.0f", GetTranslatedString(0x5ccf949a), mBountyEarned);
    FEngSetScript(mpDataBigIcon, 0x5079c8f8, true);
}

void PostRaceMilestonesScreen::StartMilestoneDoneAnimations() {
    FEngSetScript(mpDataBigIcon, 0x16a259, true);
    FEngSetScript(GetPackageName(), 0xe526d0d2, 0x33113ac, true);
    FEngSetScript(GetPackageName(), 0xe1045a4f, 0x33113ac, true);
    FEngSetScript(GetPackageName(), 0x962b9c62, FEHashUpper("POS2"), true);
    FEngSetScript(GetPackageName(), 0xec85c7e4, FEHashUpper("POS2"), true);
}

bool PostRaceMilestonesScreen::SetMilestoneAnimationScriptHash(bool isMilestone, int type) {
    const char *posStr;
    if (type == static_cast<int>(0xFD989A3A)) {
        posStr = "POS3";
    } else if (type > static_cast<int>(0xFD989A3A)) {
        if (type == 0x2377e50d) {
            posStr = "POS1";
        } else if (type > 0x2377e50d) {
            if (type == 0x4fc942ca) {
                posStr = "POS00";
            } else if (type == 0x5392e4fd) {
                posStr = "POS8";
            } else {
                mCurrMilestoneScriptHash = 0;
                goto done;
            }
        } else if (type == 0x33fa23a) {
            posStr = isMilestone ? "POS7" : "POS0";
        } else {
            mCurrMilestoneScriptHash = 0;
            goto done;
        }
    } else {
        if (type == static_cast<int>(0xA61CAC24)) {
            posStr = "POS2";
        } else if (type > static_cast<int>(0xA61CAC24)) {
            if (type == static_cast<int>(0xCDF36FC2)) {
                posStr = "POS5";
            } else if (type == static_cast<int>(0xEB45F99D)) {
                posStr = "POS4";
            } else {
                mCurrMilestoneScriptHash = 0;
                goto done;
            }
        } else {
            if (type != static_cast<int>(0x850A64BC)) {
                mCurrMilestoneScriptHash = 0;
                goto done;
            }
            posStr = "POS6";
        }
    }
    mCurrMilestoneScriptHash = FEHashUpper(posStr);
done:
    return mCurrMilestoneScriptHash != 0;
}
