#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
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
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "types.h"

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

void StatsPanel::Draw(uint32 numPlayers) {
    if (numPlayers > 1 && RacerName != nullptr && bStrCmp(RacerName, "") != 0) {
        if (!FEngIsScriptSet(ParentPkg, 0x8A41F5B9, FEHASH_APPEAR)) {
            FEngSetScript(ParentPkg, 0x8A41F5B9, FEHASH_APPEAR, true);
        }

        FEngSetButtonTexture(FEngFindImage(ParentPkg, 0x5BC), 0x5BC);
        FEngSetButtonTexture(FEngFindImage(ParentPkg, 0x682), 0x682);
        FEPrintf(ParentPkg, 0xEB43CCB0, "%s", RacerName);
    } else if (!FEngIsScriptSet(ParentPkg, 0x8A41F5B9, 0x0016A259)) {
        FEngSetScript(ParentPkg, 0x8A41F5B9, 0x0016A259, true);
    }

    for (FEWidget *widgey = TheStats.GetHead(); widgey != TheStats.EndOfList(); widgey = widgey->GetNext()) {
        widgey->Draw();
    }
}

void StatsPanel::AddStat(RaceStat *stat) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    TheStats.AddTail(stat);
    iWidgetToAdd++;
}

void StatsPanel::AddInfoStat(uint32 title, uint32 info) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    InfoStat *stat = new ("InfoStat", 0) InfoStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), title, info);
    TheStats.AddTail(stat);
    iWidgetToAdd++;
}

void StatsPanel::AddGenericStat(float stat_data, uint32 title_hash, uint32 units_hash, const char *format) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    GenericStat *stat = new ("GenericStat", 0)
        GenericStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), stat_data, title_hash, units_hash, format);
    TheStats.AddTail(stat);
    iWidgetToAdd++;
}

void StatsPanel::AddTimerStat(float seconds, uint32 title_hash) {
    FEngSetScript(ParentPkg, FEngHashString("LINE%d_GROUP", iWidgetToAdd), 0x001744B3, true);
    TimerStat *stat = new ("TimerStat", 0) TimerStat(GetCurrentString("COLUMN1_DATA"), GetCurrentString("COLUMN2_DATA"), seconds, title_hash);
    TheStats.AddTail(stat);
    iWidgetToAdd++;
}

PostRaceResultsScreen::PostRaceResultsScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), RacerStats(), RaceResults(), mNumberOfRacers(GRaceStatus::Get().GetRacerCount()), mIndexOfWinner(-1), mIndexOfCurrentRacer(-1),
      mNumberOfLaps(GRaceStatus::Get().GetRaceParameters()->GetNumLaps()), mNumberOfStats(0), mRaceType(GRaceStatus::Get().GetRaceType()),
      mPostRaceScreenMode(POSTRACESCREENMODE_RESULTS), mPlayerRacerInfo(nullptr), mMaxSlotsLeftSide(11), m_RaceButtonHash(0x5CED1D04),
      m_raceResultsUploaded(false) {
    bEnableEAMessenger = false;

    if (mRaceType == GRace::kRaceType_Tollbooth) {
        mPostRaceScreenMode = POSTRACESCREENMODE_LAPSTATS;
    }

    for (int i = 0; i < mNumberOfRacers; i++) {
        GRacerInfo *info = &GRaceStatus::Get().GetRacerInfo(i);

        if (info->GetSimable() != nullptr && mIndexOfCurrentRacer == -1 && info->GetSimable()->IsPlayer()) {
            mPlayerRacerInfo = info;
            mIndexOfCurrentRacer = i;
            break;
        }
    }

    for (int i = 0; i < 16; i++) {
        RacerStats[i].SetParentPkg(GetPackageName());
    }
    RaceResults.SetParentPkg(GetPackageName());

    Setup();
}

PostRaceResultsScreen::~PostRaceResultsScreen() {
    // g_pEAXSound->StopSpeechStream();
}

void PostRaceResultsScreen::Setup() {
    for (int i = 0; i < mNumberOfRacers; i++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(i);

        if (racerInfo.IsFinishedRacing() && racerInfo.GetRanking() == 1) {
            mIndexOfWinner = i;
            break;
        }
    }

    for (int i = 0; i < mMaxSlotsLeftSide; i++) {
        FEngSetScript(GetPackageName(), FEngHashString("LINE%d_GROUP", i + 1), 0x0016A259, true);
        if (mPostRaceScreenMode == POSTRACESCREENMODE_STATS) {
            FEngSetInvisible(GetPackageName(), FEngHashString("COLUMN2_DATA%d", i + 1));
        } else if (mPostRaceScreenMode == POSTRACESCREENMODE_LAPSTATS && mRaceType == GRace::kRaceType_Tollbooth) {
            FEngSetInvisible(GetPackageName(), FEngHashString("COLUMN2_DATA%d", i + 1));
        } else {
            FEngSetVisible(GetPackageName(), FEngHashString("COLUMN2_DATA%d", i + 1));
        }
    }

    FEngSetInvisible(GetPackageName(), 0x586AB4A6);
    FEngSetInvisible(GetPackageName(), 0x44AC8987);
    FEngSetInvisible(GetPackageName(), 0x30EE5E68);

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

    if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode() && !FEngIsScriptSet(GetPackageName(), 0x445A862B, FEHASH_APPEAR)) {
        FEngSetScript(GetPackageName(), 0x445A862B, FEHASH_APPEAR, true);
    }
}

void PostRaceResultsScreen::SetupResults() {
    FEngSetVisible(GetPackageName(), 0x586AB4A6);
    FEngSetVisible(GetPackageName(), 0x44AC8987);
    FEngSetVisible(GetPackageName(), 0x30EE5E68);

    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Drag:
        case GRace::kRaceType_Knockout:
            // case GRace::kRaceType_Tollbooth: // TODO: is this a problem?
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            break;
        case GRace::kRaceType_SpeedTrap:
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0x7540FB04);
            break;
    }

    FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xFF115FFF);
    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xD0B8AA33);

    uint32 speed_units = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speed_units = 0x8569A25F;
    }

    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Drag:
        case GRace::kRaceType_Knockout:
        case GRace::kRaceType_Tollbooth:
            for (int i = 0; i < mNumberOfRacers; i++) {
                int rank = 0;

                GRacerInfo *info = &GRaceStatus::Get().GetRacerInfo(rank);
                while (info->GetRanking() != rank) {
                    info = &GRaceStatus::Get().GetRacerInfo(rank++);
                }
                ResultStat *stat = new ("RaceResultStat", 0)
                    RaceResultStat(RaceResults.GetCurrentString("COLUMN2_DATA"), RaceResults.GetCurrentString("COLUMN3_DATA"),
                                   RaceResults.GetCurrentString("COLUMN1_DATA"), info);
                RaceResults.AddStat(stat);
            }
            break;
        case GRace::kRaceType_SpeedTrap:
            for (int i = 0; i < mNumberOfRacers; i++) {
                int rank = 0;

                GRacerInfo *info = &GRaceStatus::Get().GetRacerInfo(rank);
                while (info->GetRanking() != rank) {
                    info = &GRaceStatus::Get().GetRacerInfo(rank++);
                }

                float totalSpeed = info->GetPointTotal();
                if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
                    totalSpeed = MPS2MPH(KPH2MPS(totalSpeed));
                }

                ResultStat *stat =
                    new ("GenericResult", 0) GenericResult(RaceResults.GetCurrentString("COLUMN2_DATA"), RaceResults.GetCurrentString("COLUMN3_DATA"),
                                                           RaceResults.GetCurrentString("COLUMN1_DATA"), speed_units, totalSpeed, "%$0.0f", info);

                RaceResults.AddStat(stat);
            }
            break;
    }
}

void PostRaceResultsScreen::SetupStat_NosUsed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 mass_units = 0xC173E1BB;
    float nos = racerInfo.GetPoundsNOSUsed();

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        mass_units = 0xC173DE1F;
        nos = LB2KG(nos);
    }

#ifndef EA_BUILD_A124
    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(nos, 0x114E759F, mass_units, "%$0.0f");
        return;
    }
#endif

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x114E759F, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TopSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 speedUnits = 0x8569AB44;
    float speed = MPS2MPH(racerInfo.GetTopSpeed()); //*reinterpret_cast<const float *>(reinterpret_cast<const char *>(&racerInfo) + 0x114) * 2.23699f;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed = MPH2KPH(speed);
    }

#ifndef EA_BUILD_A124
    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x0EF34382, speedUnits, "%$0.0f");
        return;
    }
#endif

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x0EF34382, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AverageSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 speedUnits = 0x8569AB44;
    float speed = MPS2MPH(racerInfo.CalcAverageSpeed());

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed = MPH2KPH(speed);
    }

#ifndef EA_BUILD_A124
    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f && racerInfo.AreStatsReady()) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x57F4140A, speedUnits, "%$0.0f");
        return;
    }
#endif

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x57F4140A, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TimeBehind() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (mIndexOfWinner >= 0 && mIndexOfWinner != mIndexOfCurrentRacer) {
        float timeDiff = bAbs(GRaceStatus::Get().GetRacerInfo(mIndexOfWinner).GetRaceTime() - racerInfo.GetRaceTime());
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(timeDiff, 0xAB44ED8B);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0xAB44ED8B, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_LapVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (racerInfo.GetLapsCompleted() > 1) {
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
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(racerInfo.GetTrafficCarsHit(), 0x094BFDFC, 0, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x094BFDFC, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_ZeroToSixty() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 speedUnits = 0xCCBC22B3;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0xB8CF16FC;
    }

    if (racerInfo.GetZeroToSixtyTime() > 0.0f
#ifndef EA_BUILD_A124
        && racerInfo.AreStatsReady()
#endif

    ) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(racerInfo.GetZeroToSixtyTime(), speedUnits);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(speedUnits, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_QuarterMile() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 timeUnits = 0x49FD5DCB;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        timeUnits = 0x1C6F2A82;
    }

    if (racerInfo.GetQuarterMileTime() > 0.0f
#ifndef EA_BUILD_A124
        && racerInfo.AreStatsReady()
#endif
    ) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(racerInfo.GetQuarterMileTime(), timeUnits);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(timeUnits, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_PerfectShifts() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f
#ifndef EA_BUILD_A124
        && racerInfo.AreStatsReady()
#endif
    ) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(racerInfo.GetPerfectShifts(), 0x680AC597, 0, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x680AC597, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AccumulatedSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    uint32 speed_units = 0x8569A25F;
    float speed = racerInfo.GetPointTotal();

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
        speed_units = 0x8569AB44;
        speed = MPS2MPH(KPH2MPS(speed));
    }

    if (GRaceStatus::Get().GetRaceTimeElapsed() > 0.0f) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0xD57E02AB, speed_units, "%$0.0f");
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0xD57E02AB, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_SpeedVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    float bestSpeed = GRaceStatus::Get().GetBestSpeedTrapSpeed(mIndexOfCurrentRacer);
    float worstSpeed = GRaceStatus::Get().GetWorstSpeedTrapSpeed(mIndexOfCurrentRacer);

    if (bestSpeed > 0.0f && worstSpeed > 0.0f) {
        uint32 speed_units = 0x8569A25F;
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
        uint32 speed_units = 0x8569A25F;
        float speed = bAbs(GRaceStatus::Get().GetRacerInfo(mIndexOfWinner).GetPointTotal() - racerInfo.GetPointTotal());

        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
            speed_units = 0x8569AB44;
            speed = MPS2MPH(KPH2MPS(speed));
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
        case GRace::kRaceType_SpeedTrap:
            SetupStat_AccumulatedSpeed();
            SetupStat_SpeedVariance();
            SetupStat_SpeedBehind();
            break;
        case GRace::kRaceType_Tollbooth:
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
    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x8159A0B2);
    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Drag:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x34BA50FF);
            FEngSetVisible(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(GetPackageName(), 0x44AC8987);
            FEngSetVisible(GetPackageName(), 0x30EE5E68);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xE8B7D527);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            break;
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x9C8D7FE8);
            FEngSetVisible(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(GetPackageName(), 0x44AC8987);
            FEngSetVisible(GetPackageName(), 0x30EE5E68);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x0000BF9C);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            break;
        case GRace::kRaceType_SpeedTrap:
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().SortCheckPointRankings();
            }
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xECD0E6A6);
            FEngSetVisible(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(GetPackageName(), 0x44AC8987);
            FEngSetVisible(GetPackageName(), 0x30EE5E68);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xEE1EDC76);
            FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0x96B05F47);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0x7540FB04);
            break;
        case GRace::kRaceType_Tollbooth:
            FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xD10A8EA2);
            FEngSetVisible(GetPackageName(), 0x586AB4A6);
            FEngSetVisible(GetPackageName(), 0x30EE5E68);
            FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0xA15E4505);
            FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
            FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xD0B8AA33);
            break;
        default:
            break;
    }

    RacerStats[racerIndex].SetRacerName(racer_info->GetName());

    switch (mRaceType) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_Drag: {
            const uint32 numStages = 4;
            uint32 stageIndex = 0;
            while (stageIndex < numStages) {
#ifndef EA_BUILD_A124
                StageStat *stat = new ("StageStat", 0)
                    StageStat(RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                              RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), stageIndex, racer_info->GetSplitTime(stageIndex),
                              racer_info->GetSplitRanking(stageIndex));
                RacerStats[racerIndex].AddStat(stat);
#endif
                stageIndex++;
            }

            float finalRaceTime = 0.0f;
            if (racer_info->IsFinishedRacing()) {
                finalRaceTime = racer_info->GetRaceTime();
            }

            StageStat *stat = new ("StageStat", 0)
                StageStat(RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                          RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), 4, finalRaceTime, racer_info->GetRanking());
            RacerStats[racerIndex].AddStat(stat);
            break;
        }
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout: {
            bool bCumulativeTime;

            for (int lapIndex = 0; lapIndex < GRaceStatus::Get().GetRaceParameters()->GetNumLaps(); lapIndex++) {
                int lapPosition = GRaceStatus::Get().GetLapPosition(lapIndex, racerIndex, true);
                if (racer_info->GetIsKnockedOut() && lapPosition < 2) {
                    lapPosition = -1;
                }
                LapStat *stat = new ("LapStat", 0)
                    LapStat(RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                            RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), lapIndex + 1,
                            GRaceStatus::Get().GetLapTime(lapIndex, racerIndex, false), lapPosition);
                RacerStats[racerIndex].AddStat(stat);
            }
            break;
        }
        case GRace::kRaceType_SpeedTrap: {
            int trapIndex = 0;

            while (trapIndex < GRaceStatus::Get().GetNumRaceSpeedTraps()) {
                SpeedStat *stat = new ("SpeedStat", 0)
                    SpeedStat(RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                              RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), trapIndex + 1,
                              GRaceStatus::Get().GetRaceSpeedTrapSpeed(trapIndex, racerIndex),
                              GRaceStatus::Get().GetRaceSpeedTrapPosition(trapIndex, racerIndex));
                RacerStats[racerIndex].AddStat(stat);
                trapIndex++;
            }
            break;
        }
        case GRace::kRaceType_Tollbooth: {
            for (int boothIndex = 0; boothIndex < GRaceStatus::Get().GetNumRaceTollbooths(); ++boothIndex) {
                TollboothStat *stat = new ("TollboothStat", 0)
                    TollboothStat(RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                                  RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), boothIndex + 1,
                                  GRaceStatus::Get().GetRaceTollboothTime(boothIndex, racerIndex), 1);
                RacerStats[racerIndex].AddStat(stat);
            }

            float finishLineTime = 0.0f;
            if (racer_info->IsFinishedRacing()) {
                finishLineTime = GRaceStatus::Get().GetRaceTimeRemaining();
            }

            TollboothStat *stat = new ("TollboothStat", 0) TollboothStat(
                RacerStats[racerIndex].GetCurrentString("COLUMN1_DATA"), RacerStats[racerIndex].GetCurrentString("COLUMN3_DATA"),
                RacerStats[racerIndex].GetCurrentString("COLUMN2_DATA"), GRaceStatus::Get().GetNumRaceTollbooths() + 1, finishLineTime, 1);
            RacerStats[racerIndex].AddStat(stat);
            break;
        }
        default:
            break;
    }
}

// UNSOLVED (dwarf)
void PostRaceResultsScreen::NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE: {
            if (!FEDatabase->IsLANMode()) {
                if (!FEDatabase->IsOnlineMode()) {
                    return;
                }
            }

            FEngSetScript(GetPackageName(), 0x812A09D4, 0x0016A259, true);
            FEngSetScript(GetPackageName(), 0x05D85A9F, FEHASH_APPEAR, true);
            return;
        }
        case __PAD_LTRIGGER__:
            if (mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
                return;
            }

            --mIndexOfCurrentRacer;
            if (mIndexOfCurrentRacer < 0) {
                mIndexOfCurrentRacer = mNumberOfRacers - 1;
            }

            Setup();
            return;
        case __PAD_RTRIGGER__:
            if (mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
                return;
            }

            ++mIndexOfCurrentRacer;
            if (mIndexOfCurrentRacer >= mNumberOfRacers) {
                mIndexOfCurrentRacer = 0;
            }

            Setup();
            return;
        case __PAD_BUTTON4__:
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
        case __PAD_BUTTON5__: {
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
            cFEng::Get()->QueuePackageMessage(FEHASH_ENABLE_INPUT, GetPackageName(), nullptr);
            return;
        case __PAD_ACCEPT__: {
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
        }
        case 0x30ED2368:
            if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x001335F0)) {
                FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x001335F0, true);
            }
            return;
        case FEHASH_EXITCOMPLETE: {
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
                        MNotifyRaceAbandoned().Post(0x20d60dbf);
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
            return UISND_NONE;
        }
    }

    if (FEngIsScriptSet(GetPackageName(), 0x57EFB2FB, 0x0016A259)) {
        return UISND_NONE;
    }

    return maybe;
}

// Range: 0x80155F40 -> 0x801560EC
void PursuitData::PopulateData(IPursuit *ipursuit, IPerpetrator *iperpetrator, int exitToSafehouse) {
    mPursuitIsActive = (ipursuit != nullptr) && ipursuit->IsPursuitBailed() == false;

    if (ipursuit != nullptr) {
        mPursuitLength = ipursuit->GetPursuitDuration();
        mNumCopsDamaged = ipursuit->GetNumCopsDamaged();
        mNumCopsDestroyed = ipursuit->GetNumCopsDestroyed();
        mNumSpikeStripsDodged = ipursuit->GetNumSpikeStripsDodged();
        mNumRoadblocksDodged = ipursuit->GetNumRoadblocksDodged();
        mCostToStateAchieved = ipursuit->CalcTotalCostToState();
    }

    if (iperpetrator != nullptr) {
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
    if (mNumMilestonesThisPursuit >= 32) {
        return false;
    }
    mMilestonesCompleted[mNumMilestonesThisPursuit++] = milestone;
    return true;
}

const GMilestone *const PursuitData::GetMilestone(int index) const {
    if (index < 32) {
        return mMilestonesCompleted[index];
    }
    return nullptr;
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
    : ArrayDatum(0, 0), mType(type), mName(itemName), mNumber(itemNumber >= 0.0f ? itemNumber : 0.0f), mGoal(itemGoal), mChecked(itemChecked) {}

void PursuitResultsDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {}

PursuitResultsArraySlot::PursuitResultsArraySlot(FEObject *obj, FEString *itemName, FEString *itemNumber, FEImage *itemChecked, FEImage *itemEmpty)
    : ArraySlot(obj), mLine(obj), mItemName(itemName), mItemNumber(itemNumber), mItemChecked(itemChecked), mItemEmpty(itemEmpty) {}

// UNSOLVED
void PursuitResultsArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);

    if (datum != nullptr) {
        PursuitResultsDatum *result = static_cast<PursuitResultsDatum *>(datum);

        FEngSetScript(mItemChecked, FEHASH_HIDE, true);
        FEngSetScript(mItemEmpty, FEHASH_HIDE, true);
        FEngSetScript(mLine, FEHASH_INIT, true);

        if (mItemName != nullptr) {
            FEngSetScript(mItemName, FEHASH_INIT, true);
            FEngSetLanguageHash(mItemName, result->GetName());
        }

        if (result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Number) {
            FEPrintf(mItemNumber, "%$d", result->GetNumber());
            FEngSetScript(mItemNumber, FEHASH_INIT, true);
            if (result->GetChecked()) {
                FEngSetScript(mItemChecked, FEHASH_SHOW, true);
            } else if (result->GetGreyed()) {
                FEngSetScript(mItemChecked, 0x163C76, true);
            } else {
                FEngSetScript(mItemChecked, FEHASH_HIDE, true);
            }
        } else if (result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Time) {
            char timeString[32];
            Timer timeTimer(result->GetNumber());
            timeTimer.PrintToString(timeString, 0);
            FEPrintf(mItemNumber, "%s", timeString);
            FEngSetScript(mItemNumber, FEHASH_INIT, true);
            if (result->GetChecked()) {
                FEngSetScript(mItemChecked, FEHASH_SHOW, true);
            } else if (result->GetGreyed()) {
                FEngSetScript(mItemChecked, 0x163C76, true);
            } else {
                FEngSetScript(mItemChecked, FEHASH_HIDE, true);
            }
        } else if (result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number) {
            bool showCurrVal;
            char outputStr[32];
            FEDatabase->SetMilestoneDescriptionString(outputStr, -1, result->GetNumber(), result->GetGoal(), false);
            FEPrintf(mItemNumber, "%s", outputStr);
            FEngSetScript(mItemNumber, FEHASH_INIT, true);
            if (result->GetChecked()) {
                FEngSetScript(mItemChecked, FEHASH_SHOW, true);
            } else if (result->GetGreyed()) {
                FEngSetScript(mItemChecked, 0x163C76, true);
            } else {
                FEngSetScript(mItemChecked, FEHASH_HIDE, true);
            }
        } else if (result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time ||
                   result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time_PursuitRemaining) {
            bool showCurrVal;
            int milestoneType = result->GetType();
            char outputStr[32];
            FEDatabase->SetMilestoneDescriptionString(outputStr, -1, result->GetNumber(), result->GetGoal(), true);
            FEPrintf(mItemNumber, "%s", outputStr);
            FEngSetScript(mItemNumber, FEHASH_INIT, true);
            if (result->GetChecked()) {
                FEngSetScript(mItemChecked, FEHASH_SHOW, true);
            } else if (result->GetGreyed()) {
                FEngSetScript(mItemChecked, 0x163C76, true);
            } else {
                FEngSetScript(mItemChecked, FEHASH_HIDE, true);
            }
        } else if (result->GetType() == PursuitResultsDatum::PursuitResultsDatumType_Check) {
            FEngSetScript(mItemNumber, FEHASH_HIDE, true);
            if (result->GetChecked()) {
                FEngSetScript(mItemChecked, 0x163C76, true);
            } else {
                FEngSetScript(mItemChecked, 0x163C76, true);
            }
        }
    }
}

PostRacePursuitScreen::PostRacePursuitScreen(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 1, 9, false), mPostPursuitScreenMode(POSTPURSUITSCREENMODE_PURSUIT), m_RaceButtonHash(0x5CED1D04) {
    for (int i = 0; i < GetHeight(); i++) {
        char sztemp[32];

        FEngSNPrintf(sztemp, sizeof(sztemp), "LINE%d_GROUP", i + 1);
        FEObject *wrapperGroup = FEngFindObject(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "COLUMN1_DATA%d", i + 1);
        FEString *itemName = FEngFindString(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "COLUMN2_DATA%d", i + 1);
        FEString *itemValue = FEngFindString(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "CHECK%d", i + 1);
        FEImage *checkMark = FEngFindImage(GetPackageName(), FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, sizeof(sztemp), "EMPTY%d", i + 1);
        FEImage *emptyMark = FEngFindImage(GetPackageName(), FEHashUpper(sztemp));

        AddSlot(new ("PursuitResultsArraySlot", 0) PursuitResultsArraySlot(wrapperGroup, itemName, itemValue, checkMark, emptyMark));
    }
    Initialize();
}

PostRacePursuitScreen::~PostRacePursuitScreen() {
    if (GetPursuitData().mExitToSafehouse != 0) {
        GetPursuitData().mExitToSafehouse = 0;
        MEnterSafeHouse("safehouse").Post(0x20D60DBF);
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
                if ((raceParams != nullptr) && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
                    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0x9145A5F2);
                }
            }
        }
        SetupInfractions();
    } else if (mPostPursuitScreenMode == POSTPURSUITSCREENMODE_MILESTONES) {
        FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0x578B767B);
        if (GRaceStatus::Exists()) {
            GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
            if ((raceParams != nullptr) && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
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
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x4b0ff103, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Speeding)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x8ec1a6ec, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Reckless)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x370f331d, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Assault)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x8462a784, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_HitAndRun)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0xdff254ba, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Damage)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x7dbd5b34, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_Resist)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0x2b1de2a9, 0.0f, 0.0f, checkType));

    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_Off;
    if (GInfractionManager::Get().DidInfractionOccur(GInfractionManager::kInfraction_OffRoad)) {
        checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
    }
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Check, 0xb0ef5c12, 0.0f, 0.0f, checkType));
}

void PostRacePursuitScreen::SetupMilestones() {
    if (GRaceStatus::Exists()) {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
        if ((raceParams != nullptr) && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
            PursuitResultsDatum::PursuitResultsDatumType resultType = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number;
            if (FEDatabase->IsMilestoneTimeFormat(raceParams->GetChallengeType())) {
                resultType = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time;
            }
            float currVal = GManager::Get().GetBestValue(raceParams->GetChallengeType());
            float goalVal = raceParams->GetChallengeGoal();
            if (raceParams->GetChallengeType() == 0x5392e4fd) {
                resultType = PursuitResultsDatum::PursuitResultsDatumType_Time;
            }
            PursuitResultsDatum::PursuitResultsDatumCheckType checkType =
                static_cast<PursuitResultsDatum::PursuitResultsDatumCheckType>(static_cast<int>(currVal >= goalVal));
            AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(
                resultType, FEDatabase->GetChallengeHeaderHash(raceParams->GetLocalizationTag()), currVal, goalVal, checkType));
        } else {
            int currBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            GMilestone *currMilestone = GManager::Get().GetFirstMilestone(false, currBin);
            while (currMilestone != nullptr) {
                PursuitResultsDatum::PursuitResultsDatumType resultType = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number;
                if (FEDatabase->IsMilestoneTimeFormat(currMilestone->GetTypeKey())) {
                    resultType = PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time;
                }
                if (currMilestone->GetTypeKey() == 0x5392e4fd) {
                    resultType = PursuitResultsDatum::PursuitResultsDatumType_Time;
                }
                PursuitResultsDatum::PursuitResultsDatumCheckType checkType;
                if (currMilestone->GetIsDonePendingEscape()) {
                    checkType = PursuitResultsDatum::PursuitResultsDatumCheckType_On;
                } else {
                    checkType = static_cast<PursuitResultsDatum::PursuitResultsDatumCheckType>(currMilestone->GetIsAwarded() ? 2 : 0);
                }
                AddDatum(new ("PursuitResultsDatum", 0)
                             PursuitResultsDatum(resultType, FEDatabase->GetMilestoneHeaderHash(currMilestone->GetLocalizationTag()),
                                                 currMilestone->GetCurrentValue(), currMilestone->GetRequiredValue(), checkType));
                currMilestone = GManager::Get().GetNextMilestone(currMilestone, false, currBin);
            }
        }
    }
}

void PostRacePursuitScreen::SetupPursuit() {
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Time, 0x4d64888d, mPursuitData.mPursuitLength, 0.0f,
                                     PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0)
                 PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xa999f6e2,
                                     static_cast<float>(mPursuitData.mNumCopsDamaged), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x23f6e732,
                                                                static_cast<float>(mPursuitData.mNumCopsDestroyed), 0.0f,
                                                                PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x0291c816,
                                                                static_cast<float>(mPursuitData.mNumSpikeStripsDodged), 0.0f,
                                                                PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x29daba15,
                                                                static_cast<float>(mPursuitData.mNumRoadblocksDodged), 0.0f,
                                                                PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xd9bb7d2d,
                                                                static_cast<float>(mPursuitData.mCostToStateAchieved), 0.0f,
                                                                PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new ("PursuitResultsDatum", 0) PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xb7dfff96,
                                                                static_cast<float>(GInfractionManager::Get().GetNumInfractions()), 0.0f,
                                                                PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
}

void PostRacePursuitScreen::NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pObject, Param1, Param2);
    switch (msg) {
        case __PAD_ACCEPT__:
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
                        new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
                    }
                } else {
                    new EUnPause();
                }
            } else {
                new EUnPause();
            }
            break;
        case __PAD_BUTTON4__:
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

PostRaceMilestonesScreen::PostRaceMilestonesScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), mBountyEarned(0.0f), mCurrMilestoneIndex(-1), mCurrMilestoneScriptHash(0), mCopDestructionBountyShown(false) {
    mpDataBigIcon = FEngFindImage(GetPackageName(), 0x14564FB9);
}

PostRaceMilestonesScreen::~PostRaceMilestonesScreen() {}

void PostRaceMilestonesScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE:
            StartBountyAnimations(false);
            break;
        case 0xd3c3de7:
            if (!mCopDestructionBountyShown) {
                mCopDestructionBountyShown = true;
                if (PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed > 0) {
                    StartBountyAnimations(true);
                    return;
                }
            }

            if (GRaceStatus::Exists()) {
                GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
                if ((raceParams != nullptr) && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
                    StartChallengeAnimations();
                    break;
                }
            }
            StartMilestoneAnimations();
            break;
        case FEMSG_SCREEN_TICK:
            if (FEngIsScriptSet(mpDataBigIcon, FEHASH_APPEAR) && !FEngIsScriptRunning(mpDataBigIcon, FEHASH_APPEAR)) {
                FEngSetScript(mpDataBigIcon, mCurrMilestoneScriptHash, true);
            }
            break;
        case __PAD_ACCEPT__:
            cFEng::Get()->QueuePackagePop(1);
            new EShowResults(FERESULTTYPE_PURSUIT, false);
            break;
    }
}

bool PostRaceMilestonesScreen::StartMilestoneAnimations() {
    mCurrMilestoneIndex++;
    const GMilestone *const milestone = PostRacePursuitScreen::GetPursuitData().GetMilestone(mCurrMilestoneIndex);
    if (milestone != nullptr) {
        char descStr[32];
        char outputStr[64];
        FEDatabase->SetMilestoneDescriptionString(descStr, milestone->GetTypeKey(), milestone->GetCurrentValue(), milestone->GetRequiredValue(),
                                                  false);
        bSNPrintf(outputStr, sizeof(outputStr), "%s: %s", GetTranslatedString(FEDatabase->GetMilestoneHeaderHash(milestone->GetLocalizationTag())),
                  descStr);
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
        if ((raceParams != nullptr) && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {
            float currVal = GManager::Get().GetBestValue(raceParams->GetChallengeType());
            float goalVal = raceParams->GetChallengeGoal();
            char descStr[32];
            char outputStr[64];
            FEDatabase->SetMilestoneDescriptionString(descStr, raceParams->GetChallengeType(), currVal, goalVal, false);
            bSNPrintf(outputStr, sizeof(outputStr), "%s: %s",
                      GetTranslatedString(FEDatabase->GetChallengeHeaderHash(raceParams->GetLocalizationTag())), descStr);
            StartAnimations(true, raceParams->GetChallengeType(), static_cast<float>(raceParams->GetReputation()), outputStr);
            return true;
        }
    }
    StartMilestoneDoneAnimations();
    return false;
}

bool PostRaceMilestonesScreen::StartBountyAnimations(bool copDestruction) {
    Attrib::Key key;
    float bountyEarned;
    char outputStr[64];
    if (!copDestruction) {
        key = 0x33fa23a;
        bountyEarned = static_cast<float>(PostRacePursuitScreen::GetPursuitData().mRepAchievedNormal);
        bSNPrintf(outputStr, sizeof(outputStr), "%s", GetTranslatedString(0x4d64888d));
    } else {
        key = 0x4fc942ca;
        bountyEarned = static_cast<float>(PostRacePursuitScreen::GetPursuitData().mRepAchievedCopDestruction);
        bSNPrintf(outputStr, sizeof(outputStr), "%s: %$d", GetTranslatedString(0x23f6e732),
                  PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed);
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
    FEngSetScript(mpDataBigIcon, FEHASH_APPEAR, true);
}

void PostRaceMilestonesScreen::StartMilestoneDoneAnimations() {
    FEngSetScript(mpDataBigIcon, FEHASH_HIDE, true);
    FEngSetScript(GetPackageName(), 0xe526d0d2, FEHASH_LEAVE, true);
    FEngSetScript(GetPackageName(), 0xe1045a4f, FEHASH_LEAVE, true);
    FEngSetScript(GetPackageName(), 0x962b9c62, FEHashUpper("POS2"), true);
    FEngSetScript(GetPackageName(), 0xec85c7e4, FEHashUpper("POS2"), true);
}

bool PostRaceMilestonesScreen::SetMilestoneAnimationScriptHash(bool isMilestone, int type) {
    switch (type) {
        case 0x2377e50d:
            mCurrMilestoneScriptHash = FEHashUpper("POS1");
            break;
        case 0x33fa23a:
            if (isMilestone) {
                mCurrMilestoneScriptHash = FEHashUpper("POS7");
            } else {
                mCurrMilestoneScriptHash = FEHashUpper("POS0");
            }
            break;
        case static_cast<int>(0xA61CAC24):
            mCurrMilestoneScriptHash = FEHashUpper("POS2");
            break;

        case static_cast<int>(0xFD989A3A):
            mCurrMilestoneScriptHash = FEHashUpper("POS3");
            break;

        case static_cast<int>(0xEB45F99D):
            mCurrMilestoneScriptHash = FEHashUpper("POS4");
            break;

        case static_cast<int>(0xCDF36FC2):
            mCurrMilestoneScriptHash = FEHashUpper("POS5");
            break;

        case static_cast<int>(0x850A64BC):
            mCurrMilestoneScriptHash = FEHashUpper("POS6");
            break;

        case 0x4fc942ca:
            mCurrMilestoneScriptHash = FEHashUpper("POS00");
            break;

        case 0x5392e4fd:
            mCurrMilestoneScriptHash = FEHashUpper("POS8");
            break;

        default:
            mCurrMilestoneScriptHash = 0;
            break;
    }

    return mCurrMilestoneScriptHash != 0;
}
