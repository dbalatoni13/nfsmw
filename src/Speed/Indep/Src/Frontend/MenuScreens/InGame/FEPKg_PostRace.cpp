#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPKg_PostRace.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

extern FEString *FEngFindString(const char *pkg_name, int hash);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int object_hash, unsigned int language_hash);
extern int FEPrintf(FEString *text, const char *fmt, ...);

template <typename T> static T ReadField(const void *base, int offset) {
    return *reinterpret_cast< const T * >(reinterpret_cast< const char * >(base) + offset);
}

static const char *GetRacerName(const GRacerInfo *info) {
    return info != nullptr ? ReadField< const char * >(info, 0x8) : nullptr;
}

static int GetRacerRanking(const GRacerInfo *info) {
    return info != nullptr ? ReadField< int >(info, 0x10) : -1;
}

static float GetRacerDistanceDriven(const GRacerInfo *info) {
    return info != nullptr ? ReadField< float >(info, 0x110) : 0.0f;
}

static float GetRacerTopSpeed(const GRacerInfo *info) {
    return info != nullptr ? ReadField< float >(info, 0x114) : 0.0f;
}

static float GetRacerNosUsed(const GRacerInfo *info) {
    return info != nullptr ? ReadField< float >(info, 0x11C) : 0.0f;
}

static int GetRacerPerfectShifts(const GRacerInfo *info) {
    return info != nullptr ? ReadField< int >(info, 0x128) : 0;
}

static int GetRacerTrafficCollisions(const GRacerInfo *info) {
    return info != nullptr ? ReadField< int >(info, 0x12C) : 0;
}

static float GetRacerZeroToSixty(const GRacerInfo *info) {
    return info != nullptr ? ReadField< float >(info, 0x138) : 0.0f;
}

static float GetRacerQuarterMile(const GRacerInfo *info) {
    return info != nullptr ? ReadField< float >(info, 0x13C) : 0.0f;
}

RaceStat::RaceStat(FEString *title, FEString *data)
    : FEStatWidget(true) {
    SetTitleObject(title);
    SetDataObject(data);
}

RaceStat::~RaceStat() {}

RaceResultStat::~RaceResultStat() {}

void RaceResultStat::Draw() {
    FEStatWidget::Draw();
}

StageStat::~StageStat() {}

void StageStat::Draw() {
    FEStatWidget::Draw();
}

TollboothStat::~TollboothStat() {}

void TollboothStat::Draw() {
    FEStatWidget::Draw();
}

StatsPanel::StatsPanel()
    : TheStats() //
    , iWidgetToAdd(0) //
    , RacerName(nullptr) //
    , ParentPkg(nullptr) {}

StatsPanel::~StatsPanel() {
    Reset();
}

FEString *StatsPanel::GetCurrentString(const char *name) {
    if (ParentPkg == nullptr || name == nullptr) {
        return nullptr;
    }

    return FEngFindString(ParentPkg, bStringHash(name));
}

void StatsPanel::Reset() {
    TheStats.DeleteAllElements();
    iWidgetToAdd = 0;
}

void StatsPanel::Draw(unsigned int numPlayers) {
    FEWidget *widget = TheStats.GetHead();

    while (widget != TheStats.EndOfList()) {
        widget->Draw();
        widget = widget->GetNext();
    }

    if (numPlayers == 0) {
        iWidgetToAdd = 0;
    }
}

void StatsPanel::AddStat(RaceStat *stat) {
    if (stat == nullptr) {
        return;
    }

    TheStats.AddTail(stat);
    ++iWidgetToAdd;
}

void StatsPanel::AddInfoStat(unsigned int title, unsigned int info) {
    FEString *title_string = nullptr;
    FEString *info_string = nullptr;

    if (ParentPkg != nullptr) {
        if (title != 0) {
            title_string = FEngFindString(ParentPkg, title);
        }
        if (info != 0) {
            info_string = FEngFindString(ParentPkg, info);
        }
    }

    AddStat(new ("", 0) RaceStat(title_string, info_string));
}

void StatsPanel::AddGenericStat(float stat_data, unsigned int title_hash, unsigned int units_hash, const char *format) {
    FEString *title_string = nullptr;
    FEString *data_string = nullptr;

    if (ParentPkg != nullptr) {
        if (title_hash != 0) {
            title_string = FEngFindString(ParentPkg, title_hash);
        }
        if (units_hash != 0) {
            data_string = FEngFindString(ParentPkg, units_hash);
        }
    }

    if (data_string != nullptr && format != nullptr) {
        FEPrintf(data_string, format, stat_data);
    }

    AddStat(new ("", 0) RaceStat(title_string, data_string));
}

void StatsPanel::AddTimerStat(float seconds, unsigned int title_hash) {
    FEString *title_string = nullptr;
    FEString *data_string = nullptr;
    Timer time(seconds);
    char text[32];

    if (ParentPkg != nullptr) {
        if (title_hash != 0) {
            title_string = FEngFindString(ParentPkg, title_hash);
        }
        data_string = GetCurrentString("STAT_DATA");
    }

    time.PrintToString(text, sizeof(text));
    if (data_string != nullptr) {
        FEPrintf(data_string, "%s", text);
    }

    AddStat(new ("", 0) RaceStat(title_string, data_string));
}

PostRaceResultsScreen::PostRaceResultsScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , RacerStats() //
    , RaceResults() //
    , mNumberOfRacers(GRaceStatus::Get().GetRacerCount()) //
    , mIndexOfWinner(-1) //
    , mIndexOfCurrentRacer(-1) //
    , mNumberOfLaps(GRaceStatus::Get().GetRaceParameters() != nullptr ? GRaceStatus::Get().GetRaceParameters()->GetNumLaps() : 0) //
    , mNumberOfStats(0) //
    , mRaceType(GRaceStatus::Get().GetRaceType()) //
    , mPostRaceScreenMode(POSTRACESCREENMODE_RESULTS) //
    , mPlayerRacerInfo(nullptr) //
    , mMaxSlotsLeftSide(11) //
    , m_RaceButtonHash(0x5CED1D04) //
    , m_lastErrorKind(0) //
    , m_lastErrorCode(0) //
    , m_raceResultsUploaded(false) {
    if (mRaceType == GRace::kRaceType_Tollbooth) {
        mPostRaceScreenMode = POSTRACESCREENMODE_LAPSTATS;
    }

    for (int i = 0; i < 16; ++i) {
        RacerStats[i].SetParentPkg(GetPackageName());
    }
    RaceResults.SetParentPkg(GetPackageName());

    if (GRaceStatus::Exists()) {
        GRaceStatus &race_status = GRaceStatus::Get();

        for (int i = 0; i < mNumberOfRacers; ++i) {
            GRacerInfo &info = race_status.GetRacerInfo(i);
            ISimable *simable = info.GetSimable();

            if (simable != nullptr && simable->GetPlayer() != nullptr) {
                mPlayerRacerInfo = &info;
                mIndexOfCurrentRacer = i;
                break;
            }
        }

        if (mPlayerRacerInfo == nullptr) {
            mPlayerRacerInfo = race_status.GetWinningPlayerInfo();
        }

        if (mPlayerRacerInfo != nullptr) {
            mIndexOfWinner = GetRacerRanking(mPlayerRacerInfo) - 1;
        }
    }

    Setup();
}

PostRaceResultsScreen::~PostRaceResultsScreen() {
    for (int i = 15; i >= 0; --i) {
        RacerStats[i].Reset();
    }
    RaceResults.Reset();
}

void PostRaceResultsScreen::Setup() {
    if (!GRaceStatus::Exists()) {
        return;
    }

    SetupResults();
    if (mPlayerRacerInfo != nullptr && mIndexOfCurrentRacer >= 0) {
        if (mRaceType == GRace::kRaceType_Tollbooth || mRaceType == GRace::kRaceType_SpeedTrap ||
            mRaceType == GRace::kRaceType_Drag) {
            mPostRaceScreenMode = POSTRACESCREENMODE_LAPSTATS;
            SetupLapStats(mIndexOfCurrentRacer, mPlayerRacerInfo);
        } else {
            mPostRaceScreenMode = POSTRACESCREENMODE_STATS;
        }
    }
}

void PostRaceResultsScreen::SetupResults() {
    if (!GRaceStatus::Exists()) {
        return;
    }

    GRaceStatus &race_status = GRaceStatus::Get();
    FEObject *obj = FEngFindObject(GetPackageName(), 0x586AB4A6);
    FEngSetVisible(obj);
    obj = FEngFindObject(GetPackageName(), 0x44AC8987);
    FEngSetVisible(obj);
    obj = FEngFindObject(GetPackageName(), 0x30EE5E68);
    FEngSetVisible(obj);

    if (mRaceType >= GRace::kRaceType_P2P && mRaceType <= GRace::kRaceType_Knockout) {
        FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
        FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
        FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0xB67DA102);
    } else if (mRaceType == GRace::kRaceType_SpeedTrap) {
        FEngSetLanguageHash(GetPackageName(), 0x586AB4A6, 0x96B05F47);
        FEngSetLanguageHash(GetPackageName(), 0x44AC8987, 0xCE678AD3);
        FEngSetLanguageHash(GetPackageName(), 0x30EE5E68, 0x7540FB04);
    }

    FEngSetLanguageHash(GetPackageName(), 0x2D691760, 0xFF115FFF);
    FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xD0B8AA33);
    mNumberOfStats = 0;
    RaceResults.Reset();

    for (int i = 0; i < mNumberOfRacers && i < 16; ++i) {
        SetupRacerStats(i, &race_status.GetRacerInfo(i));
    }

    if (mPlayerRacerInfo == nullptr) {
        return;
    }

    SetupStat_NosUsed();
    SetupStat_TopSpeed();
    SetupStat_AverageSpeed();
    SetupStat_TimeBehind();
    SetupStat_LapVariance();
    SetupStat_StageVariance();
    SetupStat_TrafficCollisions();
    SetupStat_ZeroToSixty();
    SetupStat_QuarterMile();
    SetupStat_PerfectShifts();
    SetupStat_AccumulatedSpeed();
    SetupStat_SpeedVariance();
    SetupStat_SpeedBehind();
}

void PostRaceResultsScreen::SetupStat_NosUsed() {
    RaceResults.AddGenericStat(GetRacerNosUsed(mPlayerRacerInfo), 0, 0, "%.1f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_TopSpeed() {
    RaceResults.AddGenericStat(GetRacerTopSpeed(mPlayerRacerInfo), 0, 0, "%.1f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_AverageSpeed() {
    RaceResults.AddGenericStat(mPlayerRacerInfo != nullptr ? mPlayerRacerInfo->GetFinishingSpeed() : 0.0f, 0, 0, "%.1f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_TimeBehind() {
    RaceResults.AddTimerStat(0.0f, 0);
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_LapVariance() {
    if (GRaceStatus::Exists() && mIndexOfCurrentRacer >= 0) {
        GRaceStatus &race_status = GRaceStatus::Get();
        RaceResults.AddTimerStat(race_status.GetWorstLapTime(mIndexOfCurrentRacer) - race_status.GetBestLapTime(mIndexOfCurrentRacer), 0);
    } else {
        RaceResults.AddTimerStat(0.0f, 0);
    }
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_StageVariance() {
    RaceResults.AddTimerStat(0.0f, 0);
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_TrafficCollisions() {
    RaceResults.AddGenericStat(static_cast< float >(GetRacerTrafficCollisions(mPlayerRacerInfo)), 0, 0, "%.0f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_ZeroToSixty() {
    RaceResults.AddTimerStat(GetRacerZeroToSixty(mPlayerRacerInfo), 0);
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_QuarterMile() {
    RaceResults.AddTimerStat(GetRacerQuarterMile(mPlayerRacerInfo), 0);
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_PerfectShifts() {
    RaceResults.AddGenericStat(static_cast< float >(GetRacerPerfectShifts(mPlayerRacerInfo)), 0, 0, "%.0f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_AccumulatedSpeed() {
    RaceResults.AddGenericStat(GetRacerDistanceDriven(mPlayerRacerInfo), 0, 0, "%.1f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_SpeedVariance() {
    if (GRaceStatus::Exists() && mIndexOfCurrentRacer >= 0) {
        GRaceStatus &race_status = GRaceStatus::Get();
        RaceResults.AddGenericStat(race_status.GetBestLapTime(mIndexOfCurrentRacer) - race_status.GetWorstLapTime(mIndexOfCurrentRacer), 0, 0, "%.2f");
    } else {
        RaceResults.AddGenericStat(0.0f, 0, 0, "%.2f");
    }
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupStat_SpeedBehind() {
    float speed_behind = 0.0f;

    if (GRaceStatus::Exists() && mNumberOfRacers > 0 && mPlayerRacerInfo != nullptr) {
        GRaceStatus &race_status = GRaceStatus::Get();
        GRacerInfo &winner_info = race_status.GetRacerInfo(0);
        speed_behind = GetRacerTopSpeed(&winner_info) - GetRacerTopSpeed(mPlayerRacerInfo);
    }

    RaceResults.AddGenericStat(speed_behind, 0, 0, "%.1f");
    ++mNumberOfStats;
}

void PostRaceResultsScreen::SetupRacerStats(int index, GRacerInfo *racer_info) {
    if (index < 0 || index >= 16) {
        return;
    }

    StatsPanel &panel = RacerStats[index];
    panel.Reset();

    if (racer_info == nullptr) {
        return;
    }

    panel.SetRacerName(GetRacerName(racer_info));
    panel.AddStat(new ("", 0) RaceResultStat(nullptr, nullptr, nullptr, racer_info));
    panel.AddGenericStat(GetRacerTopSpeed(racer_info), 0, 0, "%.1f");
    panel.AddGenericStat(static_cast< float >(GetRacerRanking(racer_info)), 0, 0, "%.0f");
}

void PostRaceResultsScreen::SetupLapStats(int racerIndex, GRacerInfo *racer_info) {
    if (!GRaceStatus::Exists() || racer_info == nullptr) {
        return;
    }

    GRaceStatus &race_status = GRaceStatus::Get();
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

    RaceResults.Reset();

    for (int i = 0; i < mNumberOfLaps; ++i) {
        switch (mRaceType) {
        case GRace::kRaceType_Tollbooth:
            RaceResults.AddStat(new ("", 0) TollboothStat(nullptr, nullptr, nullptr, i + 1, race_status.GetRaceTollboothTime(i, racerIndex), 0));
            break;
        case GRace::kRaceType_SpeedTrap:
            RaceResults.AddGenericStat(race_status.GetRaceSpeedTrapSpeed(i, racerIndex), 0, 0, "%.1f");
            break;
        default:
            RaceResults.AddStat(new ("", 0) StageStat(nullptr, nullptr, nullptr, i + 1, race_status.GetLapTime(i, racerIndex, false), race_status.GetLapPosition(i, racerIndex, false)));
            break;
        }
    }
}

void PostRaceResultsScreen::NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long Param1, unsigned long Param2) {
    MenuScreen::NotificationMessage(msg, pObject, Param1, Param2);

    if (msg == 0xC98356BA) {
        Setup();
    }
}

eMenuSoundTriggers PostRaceResultsScreen::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return MenuScreen::NotifySoundMessage(msg, maybe);
}

static MenuScreen *CreatePostRaceResultsScreen(ScreenConstructorData *sd) {
    return new ("", 0) PostRaceResultsScreen(sd);
}
