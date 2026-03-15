#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPKg_PostRace.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceAbandoned.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GTimer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern FEString *FEngFindString(const char *pkg_name, int hash);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetVisible(FEObject *obj);
extern bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash,
                          bool start_at_beginning);
extern void FEngSetScript(FEObject *obj, unsigned int script_hash, bool start_at_beginning);
extern void FEngSetLanguageHash(FEString *text, unsigned int hash);
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int object_hash, unsigned int language_hash);
extern unsigned int FEngHashString(const char *, ...);
extern int FEPrintf(FEString *text, const char *fmt, ...);
extern const char *GetLocalizedPercentSign();
extern float GRaceStatusGetRaceTimeElapsed(const GRaceStatus *race_status) asm("GetRaceTimeElapsed__C11GRaceStatus");
extern float GRacerInfoCalcAverageSpeed(const GRacerInfo *racer_info) asm("CalcAverageSpeed__C10GRacerInfo");
extern bool GRacerInfoAreStatsReady(const GRacerInfo *racer_info) asm("AreStatsReady__C10GRacerInfo");

extern const char lbl_803E4CB4[];  // "%d"
extern const char lbl_803E4CF0[];  // "%s"

int bSNPrintf(char *buf, int max_len, const char *format, ...);
const char *GetLocalizedString(unsigned int hash);
extern const char lbl_803E5074[];
extern const char lbl_803E507C[];
extern const char lbl_803E5084[];
extern const char lbl_803E5088[];
extern const char lbl_803E5E44[];
extern const float lbl_803E5E54;
extern const float lbl_803E5E58;
extern const float lbl_803E5E5C;
extern const float lbl_803E5E60;
extern const float lbl_803E5E64;
extern const float lbl_803E5E68;
extern const float lbl_803E5E6C;
extern const float lbl_803E5E70;
extern const float lbl_803E5E74;
extern const double lbl_803E5E78;
extern const float lbl_803E5E80;
extern const float lbl_803E5E84;
extern const float lbl_803E5E88;
extern const double lbl_803E5E90;
extern const float lbl_803E5E98;
extern const float lbl_803E5E9C;
extern const float lbl_803E5EA0;
extern const float lbl_803E5EB0;
extern const float lbl_803E5EB4;
extern const char lbl_803E5DB0[];
extern const char lbl_803E5DCC[];
extern const char lbl_803E5DDC[];
extern const char lbl_803E5E04[];
extern const char lbl_803E5E24[];
extern const char lbl_803E5EEC[];
extern const char lbl_803E5F00[];
extern const char lbl_803E5F18[];

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

static float GetRacerStageTime(const GRacerInfo *info, int index) {
    return info != nullptr ? ReadField< float >(info, 0x140 + index * 4) : 0.0f;
}

static int GetRacerStagePosition(const GRacerInfo *info, int index) {
    return info != nullptr ? ReadField< int >(info, 0x150 + index * 4) : 0;
}

static float GetRacerTotalStageTime(const GRacerInfo *info) {
    if (info == nullptr || ReadField< int >(info, 0x30) == 0) {
        return 0.0f;
    }

    return ReadField< const GTimer >(info, 0x160).GetTime();
}

static FEString *GetPanelString(StatsPanel &panel, const char *label) {
    return FEngFindString(panel.ParentPkg, FEngHashString(lbl_803E5088, label, panel.RacerName));
}

struct LapStat : public ResultStat {
    LapStat(FEString *lap, FEString *time, FEString *pos, int lap_num, float seconds, int pos_num)
        : ResultStat(lap, time, pos, nullptr) //
        , LapNum(lap_num) //
        , Time(seconds) //
        , PosNum(pos_num) {}

    ~LapStat() override;
    void Draw() override;

    int LapNum;
    Timer Time;
    int PosNum;
};

struct SpeedStat : public ResultStat {
    SpeedStat(FEString *speedtrap, FEString *speed, FEString *pos, int trap_num, float trap_speed, int pos_num)
        : ResultStat(speedtrap, speed, pos, nullptr) //
        , TrapNum(trap_num) //
        , Speed(trap_speed) //
        , PosNum(pos_num) {}

    ~SpeedStat() override;
    void Draw() override;

    int TrapNum;
    float Speed;
    int PosNum;
};

RaceStat::RaceStat(FEString *title, FEString *data)
    : FEStatWidget(true) {
    SetTitleObject(title);
    SetDataObject(data);
}

RaceResultStat::~RaceResultStat() {}

void RaceResultStat::Draw() {
    char text[0x20];

    FEPrintf(GetTitleObject(), lbl_803E4CF0, RacerInfo->GetName());

    if (RacerInfo->GetIsEngineBlown()) {
        FEngSetLanguageHash(GetDataObject(), 0x01E66364);
    } else if (RacerInfo->GetIsTotalled()) {
        FEngSetLanguageHash(GetDataObject(), 0xB7B75185);
    } else if (RacerInfo->GetIsKnockedOut()) {
        FEngSetLanguageHash(GetDataObject(), 0x5D82DBA2);
    } else if (!RacerInfo->IsFinishedRacing()) {
        FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
    } else {
        Timer t(RacerInfo->GetRaceTimer().GetTime());
        t.PrintToString(text, 0);
        FEPrintf(GetDataObject(), lbl_803E4CF0, text);
    }

    FEPrintf(Position, lbl_803E4CB4, RacerInfo->GetRanking());
}

LapStat::~LapStat() {}

void LapStat::Draw() {
    FEPrintf(GetTitleObject(), lbl_803E4CB4, LapNum);
    if (static_cast< float >(PosNum) == 0.0f) {
        FEngSetLanguageHash(Position, 0x5D82DBA2);
        FEngSetLanguageHash(GetDataObject(), 0x5D82DBA2);
        return;
    }

    if (Time.GetSeconds() == 0.0f) {
        FEngSetLanguageHash(Position, 0x0FC1BF40);
        FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
        return;
    }

    char text[40];
    FEPrintf(Position, lbl_803E4CB4, PosNum);
    Time.PrintToString(text, 0);
    FEPrintf(GetDataObject(), lbl_803E4CF0, text);
}

StageStat::~StageStat() {}

void StageStat::Draw() {
    FEPrintf(GetTitleObject(), lbl_803E5074, (StageNum + 1) * 0x14, GetLocalizedPercentSign());
    if (Time.GetSeconds() == 0.0f) {
        FEngSetLanguageHash(Position, 0x0FC1BF40);
        FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
        return;
    }

    char text[32];
    FEPrintf(Position, lbl_803E4CB4, PosNum);
    Time.PrintToString(text, 0);
    FEPrintf(GetDataObject(), lbl_803E4CF0, text);
}

SpeedStat::~SpeedStat() {}

void SpeedStat::Draw() {
    FEPrintf(GetTitleObject(), lbl_803E4CB4, TrapNum);
    if (Speed == 0.0f) {
        FEngSetLanguageHash(Position, 0x0FC1BF40);
        FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
        return;
    }

    float scale = 2.23699f;
    if (ReadField< unsigned char >(ReadField< void * >(FEDatabase, 0x20), 0x44) == 1) {
        scale = 3.6f;
    }

    FEPrintf(Position, lbl_803E4CB4, PosNum);
    FEPrintf(GetDataObject(), lbl_803E507C, Speed * scale);
}

TollboothStat::~TollboothStat() {}

void TollboothStat::Draw() {
    FEPrintf(GetTitleObject(), lbl_803E4CB4, TollboothNum);
    if (static_cast< float >(PosNum) == 1.0f) {
        FEngSetLanguageHash(Position, 0x5D82DBA2);
        FEngSetLanguageHash(GetDataObject(), 0x5D82DBA2);
        return;
    }

    if (Time.GetSeconds() == 0.0f) {
        FEngSetLanguageHash(Position, 0x0FC1BF40);
        FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
        return;
    }

    char text[32];
    FEPrintf(Position, lbl_803E4CB4, PosNum);
    Time.PrintToString(text, 0);
    FEPrintf(GetDataObject(), lbl_803E5084, text);
}

StatsPanel::StatsPanel()
    : TheStats() //
    , iWidgetToAdd(0) //
    , RacerName(nullptr) //
    , ParentPkg(nullptr) {}

StatsPanel::~StatsPanel() {
}

FEString *StatsPanel::GetCurrentString(const char *name) {
    if (ParentPkg == nullptr || name == nullptr) {
        return nullptr;
    }

    return FEngFindString(ParentPkg, bStringHash(name));
}

void StatsPanel::Reset() {
    TheStats.DeleteAllElements();
    iWidgetToAdd = 1;
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

    GRaceStatus &race_status = GRaceStatus::Get();

    for (int i = 0; i < mNumberOfRacers; ++i) {
        GRacerInfo &info = race_status.GetRacerInfo(i);

        if (ReadField< bool >(&info, 0x30) && GetRacerRanking(&info) == 1) {
            mIndexOfWinner = i;
            break;
        }
    }

    unsigned int script_hash = FEngHashString(lbl_803E5DB0);

    for (int i = 1; i <= mMaxSlotsLeftSide; ++i) {
        FEngSetScript(GetPackageName(), script_hash, 0x0016A259, true);

        FEObject *obj = FEngFindObject(GetPackageName(), FEngHashString(lbl_803E5E04, i));

        if (mPostRaceScreenMode == POSTRACESCREENMODE_STATS ||
            (mPostRaceScreenMode == POSTRACESCREENMODE_LAPSTATS && mRaceType == GRace::kRaceType_Tollbooth)) {
            FEngSetInvisible(obj);
        } else {
            FEngSetVisible(obj);
        }
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
        if (mIndexOfCurrentRacer >= 0 && mPlayerRacerInfo != nullptr) {
            RacerStats[mIndexOfCurrentRacer].Reset();
            SetupRacerStats(mIndexOfCurrentRacer, mPlayerRacerInfo);
            RacerStats[mIndexOfCurrentRacer].Draw(mNumberOfRacers);
        }
        break;
    case POSTRACESCREENMODE_LAPSTATS:
        if (mIndexOfCurrentRacer >= 0 && mPlayerRacerInfo != nullptr) {
            RacerStats[mIndexOfCurrentRacer].Reset();
            SetupLapStats(mIndexOfCurrentRacer, mPlayerRacerInfo);
            RacerStats[mIndexOfCurrentRacer].Draw(mNumberOfRacers);
        }
        break;
    default:
        break;
    }

    unsigned int fe_flags = ReadField< unsigned int >(FEDatabase, 0x1C0);

    if ((fe_flags & 8) == 0 && (fe_flags & 0x40) == 0 &&
        !FEngIsScriptSet(GetPackageName(), 0x445A862B, 0x5079C8F8)) {
        FEngSetScript(GetPackageName(), 0x445A862B, 0x5079C8F8, true);
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

    for (int place = 1; place <= mNumberOfRacers; ++place) {
        GRacerInfo *racer_info = nullptr;

        for (int i = 0; i < mNumberOfRacers; ++i) {
            GRacerInfo &info = race_status.GetRacerInfo(i);

            if (GetRacerRanking(&info) == place) {
                racer_info = &info;
                break;
            }
        }

        if (racer_info != nullptr) {
            RaceResults.AddStat(new ("", 0) RaceResultStat(nullptr, nullptr, nullptr, racer_info));
        }
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
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int mass_units = 0xC173E1BB;
    float nos = *reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x11C);

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        mass_units = 0xC173DE1F;
        nos *= lbl_803E5E54;
    }

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5E58 && GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(nos, 0x114E759F, mass_units, lbl_803E5E44);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x114E759F, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TopSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569AB44;
    float speed = *reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x114) * lbl_803E5E5C;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed *= lbl_803E5E60;
    }

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5E64 && GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x0EF34382, speedUnits, lbl_803E5E44);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x0EF34382, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AverageSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569AB44;
    float speed = GRacerInfoCalcAverageSpeed(&racerInfo) * lbl_803E5E68;

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
        speed *= lbl_803E5E6C;
    }

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5E70 && GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x57F4140A, speedUnits, lbl_803E5E44);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x57F4140A, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_TimeBehind() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (mIndexOfWinner >= 0 && mIndexOfWinner != mIndexOfCurrentRacer) {
        GRacerInfo &winnerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfWinner);
        float winnerTime =
            reinterpret_cast< const GTimer * >(reinterpret_cast< const char * >(&winnerInfo) + 0x160)->GetTime();
        float racerTime =
            reinterpret_cast< const GTimer * >(reinterpret_cast< const char * >(&racerInfo) + 0x160)->GetTime();

        RacerStats[mIndexOfCurrentRacer].AddTimerStat(bAbs(winnerTime - racerTime), 0xAB44ED8B);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0xAB44ED8B, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_LapVariance() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (*reinterpret_cast< const int * >(reinterpret_cast< const char * >(&racerInfo) + 0x3C) > 1) {
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

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5E74) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(
            static_cast< float >(*reinterpret_cast< const int * >(reinterpret_cast< const char * >(&racerInfo) + 0x12C)),
            0x094BFDFC, 0, lbl_803E5E44);
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

    if (*reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x138) > lbl_803E5E80 &&
        GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(
            *reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x138), speedUnits);
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

    if (*reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x13C) > lbl_803E5E84 &&
        GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddTimerStat(
            *reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x13C), timeUnits);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(timeUnits, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_PerfectShifts() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5E88 && GRacerInfoAreStatsReady(&racerInfo)) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(
            static_cast< float >(*reinterpret_cast< const int * >(reinterpret_cast< const char * >(&racerInfo) + 0x128)),
            0x680AC597, 0, lbl_803E5E44);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x680AC597, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupStat_AccumulatedSpeed() {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(mIndexOfCurrentRacer);
    unsigned int speedUnits = 0x8569A25F;
    float speed = *reinterpret_cast< const float * >(reinterpret_cast< const char * >(&racerInfo) + 0x134);

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
        speedUnits = 0x8569AB44;
        speed = (speed * lbl_803E5E98) * lbl_803E5E9C;
    }

    if (GRaceStatusGetRaceTimeElapsed(&GRaceStatus::Get()) > lbl_803E5EA0) {
        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0xD57E02AB, speedUnits, lbl_803E5E44);
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
            speed = (speed * lbl_803E5EB0) * lbl_803E5EB4;
        }

        RacerStats[mIndexOfCurrentRacer].AddGenericStat(speed, 0x2E54B7ED, speed_units, lbl_803E5E44);
        return;
    }

    RacerStats[mIndexOfCurrentRacer].AddInfoStat(0x2E54B7ED, 0x0FC1BF40);
}

void PostRaceResultsScreen::SetupRacerStats(int index, GRacerInfo *racer_info) {
    StatsPanel &panel = RacerStats[index];

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
        FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xAC23368C);
        break;
    case GRace::kRaceType_Tollbooth:
        FEngSetLanguageHash(GetPackageName(), m_RaceButtonHash, 0xAEF51E9D);
        break;
    default:
        break;
    }

    panel.RacerName = GetRacerName(racer_info);
    unsigned int fe_flags = ReadField< unsigned int >(FEDatabase, 0x1C0);

    switch (mRaceType) {
    case GRace::kRaceType_Drag:
        SetupStat_ZeroToSixty();
        SetupStat_QuarterMile();
        SetupStat_PerfectShifts();
        if ((fe_flags & 0x40) != 0 || (fe_flags & 8) != 0) {
            SetupStat_NosUsed();
        } else {
            SetupStat_TimeBehind();
            SetupStat_TrafficCollisions();
        }
        break;
    case GRace::kRaceType_P2P:
        SetupStat_TopSpeed();
        SetupStat_AverageSpeed();
        if ((fe_flags & 0x40) == 0 && (fe_flags & 8) == 0) {
            SetupStat_TimeBehind();
            SetupStat_TrafficCollisions();
        } else {
            SetupStat_NosUsed();
        }
        SetupStat_StageVariance();
        break;
    case GRace::kRaceType_Circuit:
    case GRace::kRaceType_Knockout:
        SetupStat_TopSpeed();
        SetupStat_AverageSpeed();
        if ((fe_flags & 0x40) != 0 || (fe_flags & 8) != 0) {
            SetupStat_NosUsed();
        } else {
            SetupStat_TimeBehind();
            SetupStat_LapVariance();
            SetupStat_TrafficCollisions();
        }
        break;
    case GRace::kRaceType_SpeedTrap:
        SetupStat_TopSpeed();
        SetupStat_AverageSpeed();
        SetupStat_StageVariance();
        SetupStat_TrafficCollisions();
        break;
    case GRace::kRaceType_Tollbooth:
        SetupStat_AccumulatedSpeed();
        SetupStat_SpeedVariance();
        SetupStat_SpeedBehind();
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

    RacerStats[racerIndex].RacerName = ReadField< const char * >(racer_info, 0x8);

    GRaceStatus &race_status = GRaceStatus::Get();
    StatsPanel &panel = RacerStats[racerIndex];

    switch (mRaceType) {
    case GRace::kRaceType_P2P:
    case GRace::kRaceType_Drag:
        for (int i = 0; i < 4; ++i) {
            panel.AddStat(new ("", 0)
                              StageStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                        GetPanelString(panel, lbl_803E5E24), i, GetRacerStageTime(racer_info, i),
                                        GetRacerStagePosition(racer_info, i)));
        }

        panel.AddStat(new ("", 0)
                          StageStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                    GetPanelString(panel, lbl_803E5E24), 4, GetRacerTotalStageTime(racer_info),
                                    GetRacerRanking(racer_info)));
        break;
    case GRace::kRaceType_Circuit:
    case GRace::kRaceType_Knockout:
        for (int i = 0; i < race_status.GetRaceParameters()->GetNumLaps(); ++i) {
            int lap_position = race_status.GetLapPosition(i, racerIndex, true);

            if (ReadField< int >(racer_info, 0x1C) != 0 && lap_position < 2) {
                lap_position = -1;
            }

            panel.AddStat(new ("", 0)
                              LapStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                      GetPanelString(panel, lbl_803E5E24), i + 1,
                                      race_status.GetLapTime(i, racerIndex, false), lap_position));
        }
        break;
    case GRace::kRaceType_Tollbooth: {
        unsigned int num_booths =
            race_status.GetRaceParameters() != nullptr ? race_status.GetRaceParameters()->GetNumCheckpoints() : 0;

        for (unsigned int i = 0; i < num_booths; ++i) {
            panel.AddStat(new ("", 0)
                              TollboothStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                            GetPanelString(panel, lbl_803E5E24), i + 1,
                                            race_status.GetRaceTollboothTime(i, racerIndex), 1));
        }

        panel.AddStat(new ("", 0)
                          TollboothStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                        GetPanelString(panel, lbl_803E5E24), num_booths + 1,
                                        ReadField< int >(racer_info, 0x30) != 0 ? race_status.GetRaceTimeRemaining()
                                                                                : 0.0f,
                                        1));
        break;
    }
    case GRace::kRaceType_SpeedTrap: {
        unsigned int num_traps = GManager::Exists() ? GManager::Get().GetNumSpeedTraps() : 0;

        for (unsigned int i = 0; i < num_traps; ++i) {
            panel.AddStat(new ("", 0)
                              SpeedStat(GetPanelString(panel, lbl_803E5DCC), GetPanelString(panel, lbl_803E5DDC),
                                        GetPanelString(panel, lbl_803E5E24), i + 1,
                                        race_status.GetRaceSpeedTrapSpeed(i, racerIndex),
                                        race_status.GetRaceSpeedTrapPosition(i, racerIndex)));
        }
        break;
    }
    default:
        break;
    }
}

void PostRaceResultsScreen::NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long Param1,
                                                unsigned long Param2) {
    switch (msg) {
    case 0x35F8620B: {
        unsigned int fe_flags =
            *reinterpret_cast< const unsigned int * >(reinterpret_cast< const char * >(FEDatabase) + 0x1C0);

        if ((fe_flags & 0x40) == 0) {
            if ((fe_flags & 8) == 0) {
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
            mPostRaceScreenMode = static_cast< PostRaceScreenMode >(mPostRaceScreenMode + 1);
            if (mPostRaceScreenMode == POSTRACESCREENMODE_NUMMODES) {
                mPostRaceScreenMode = POSTRACESCREENMODE_RESULTS;
            }
        }

        Setup();
        return;
    case 0xC519BFC4: {
        unsigned int fe_flags =
            *reinterpret_cast< const unsigned int * >(reinterpret_cast< const char * >(FEDatabase) + 0x1C0);

        if ((fe_flags & 0x40) != 0) {
            return;
        }

        if ((fe_flags & 8) != 0) {
            return;
        }

        DialogInterface::ShowTwoButtons(GetPackageName(), lbl_803E5EEC, static_cast< eDialogTitle >(1), 0x417B2601,
                                        0x1A294DAD, 0xE1A57D51, 0xB4623F67, 0xB4623F67,
                                        static_cast< eDialogFirstButtons >(1),
                                        static_cast< unsigned int >(0x4D3399A8));
        return;
    }
    case 0xE1A57D51:
        new EUnPause();
        if (cFEng::Get()->IsPackagePushed(lbl_803E5F00)) {
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

        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career &&
            GRaceStatus::Get().GetRaceParameters()->GetIsBossRace()) {
            bool show_dialog = false;
            const char *player_racer_info = reinterpret_cast< const char * >(mPlayerRacerInfo);

            if (player_racer_info != nullptr) {
                if (*reinterpret_cast< const int * >(player_racer_info + 0x30) != 0 ||
                    *reinterpret_cast< const int * >(player_racer_info + 0x20) != 0 ||
                    *reinterpret_cast< const int * >(player_racer_info + 0x24) != 0 ||
                    *reinterpret_cast< const int * >(player_racer_info + 0x1C) != 0 ||
                    *reinterpret_cast< const int * >(player_racer_info + 0x28) != 0) {
                    show_dialog = true;
                }
            }

            if (show_dialog && *reinterpret_cast< const int * >(player_racer_info + 0x10) != 1) {
                DialogInterface::ShowTwoButtons(GetPackageName(), lbl_803E5EEC, static_cast< eDialogTitle >(1),
                                                0x417B2601, 0x1A294DAD, 0x30ED2368, 0xB4623F67, 0xB4623F67,
                                                static_cast< eDialogFirstButtons >(1),
                                                static_cast< unsigned int >(0x9887EB98));
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
        unsigned int fe_flags =
            *reinterpret_cast< const unsigned int * >(reinterpret_cast< const char * >(FEDatabase) + 0x1C0);

        if ((fe_flags & 0x40) != 0) {
            return;
        }

        if ((fe_flags & 8) != 0) {
            return;
        }

        if (cFEng::Get()->IsPackagePushed(lbl_803E5F00)) {
            cFEng::Get()->QueuePackagePop(1);
            if (cFEng::Get()->IsPackagePushed(lbl_803E5F18)) {
                cFEng::Get()->QueuePackagePop(1);
            }
            return;
        }

        bool has_race_data = false;
        const char *player_racer_info = reinterpret_cast< const char * >(mPlayerRacerInfo);
        if (player_racer_info != nullptr) {
            if (*reinterpret_cast< const int * >(player_racer_info + 0x30) != 0 ||
                *reinterpret_cast< const int * >(player_racer_info + 0x20) != 0 ||
                *reinterpret_cast< const int * >(player_racer_info + 0x24) != 0 ||
                *reinterpret_cast< const int * >(player_racer_info + 0x1C) != 0 ||
                *reinterpret_cast< const int * >(player_racer_info + 0x28) != 0) {
                has_race_data = true;
            }
        }

        bool is_dday_race = false;
        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr &&
            GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
            is_dday_race = true;
        }

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career &&
            !is_dday_race) {
            if (has_race_data) {
                GRaceStatus::Get().RaceAbandoned();

                MNotifyRaceAbandoned abandoned;
                abandoned.Post(MNotifyRaceAbandoned::_GetKind());
            }

            new EUnPause();
            return;
        }

        if (has_race_data) {
            new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
        } else {
            new EUnPause();
        }
        return;
    }
    default:
        return;
    }
}

eMenuSoundTriggers PostRaceResultsScreen::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg != 0x7B6B89D7) {
        if (msg < 0x7B6B89D8) {
            if (msg != 0x4A805994) {
                return maybe;
            }
        } else if (msg != 0x9AFA53A7) {
            return maybe;
        }

        if (mNumberOfRacers < 2 || mPostRaceScreenMode == POSTRACESCREENMODE_RESULTS) {
            return static_cast< eMenuSoundTriggers >( -1 );
        }
    }

    if (FEngIsScriptSet(GetPackageName(), 0x57EFB2FB, 0x0016A259)) {
        return static_cast< eMenuSoundTriggers >( -1 );
    }

    return maybe;
}

static MenuScreen *CreatePostRaceResultsScreen(ScreenConstructorData *sd) {
    return new ("", 0) PostRaceResultsScreen(sd);
}

bool PursuitData::AddMilestone(GMilestone *milestone) {
    if (mNumMilestonesThisPursuit < 0x20) {
        mMilestonesCompleted[mNumMilestonesThisPursuit] = milestone;
        mNumMilestonesThisPursuit = mNumMilestonesThisPursuit + 1;
        return true;
    }
    return false;
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

void InfoStat::Draw() {
    FEngSetLanguageHash(GetTitleObject(), TitleHash);
    FEngSetLanguageHash(GetDataObject(), InfoHash);
}

void GenericStat::Draw() {
    char text[0x20];
    FEngSetLanguageHash(GetTitleObject(), TitleHash);
    bSNPrintf(text, 0x20, Format, StatData);
    if (UnitsHash != 0) {
        FEString *data = GetDataObject();
        const char *units = GetLocalizedString(UnitsHash);
        FEPrintf(data, "%s %s", text, units);
    } else {
        FEPrintf(GetDataObject(), lbl_803E4CF0, text);
    }
}

void TimerStat::Draw() {
    char text[0x20];
    FEngSetLanguageHash(GetTitleObject(), TitleHash);
    Seconds.PrintToString(text, 0);
    FEPrintf(GetDataObject(), lbl_803E4CF0, text);
}

void GenericResult::Draw() {
    char text[0x20];

    FEPrintf(GetTitleObject(), lbl_803E4CF0, RacerInfo->GetName());

    if (!RacerInfo->IsFinishedRacing()) {
        bool showData = false;
        if (GRaceStatus::Exists()) {
            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                showData = true;
            }
        }
        if (!showData) {
            FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
            goto show_position;
        }
    }

    bSNPrintf(text, 0x20, Format, FData);
    if (UnitsHash != 0) {
        FEString *data = GetDataObject();
        const char *units = GetLocalizedString(UnitsHash);
        FEPrintf(data, "%s %s", text, units);
    } else {
        FEPrintf(GetDataObject(), lbl_803E4CF0, text);
    }

show_position:
    FEPrintf(Position, lbl_803E4CB4, RacerInfo->GetRanking());
}

FEImage *FEngFindImage(const char *pkg_name, int name_hash);

PostRaceMilestonesScreen::PostRaceMilestonesScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , mBountyEarned(0.0f) //
    , mCurrMilestoneIndex(-1) //
    , mCurrMilestoneScriptHash(0) //
    , mCopDestructionBountyShown(false) //
{
    mpDataBigIcon = FEngFindImage(GetPackageName(), 0x14564FB9);
}

PostRaceMilestonesScreen::~PostRaceMilestonesScreen() {}

extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern const char *GetTranslatedString(unsigned int hash);
int FEPrintf(const char *pkg_name, int object_hash, const char *fmt, ...);

void PostRaceMilestonesScreen::StartMilestoneDoneAnimations() {
    FEngSetScript(mpDataBigIcon, 0x16a259, true);
    FEngSetScript(GetPackageName(), 0xe526d0d2, 0x33113ac, true);
    FEngSetScript(GetPackageName(), 0xe1045a4f, 0x33113ac, true);
    unsigned int posHash = FEHashUpper("POS2");
    FEngSetScript(GetPackageName(), 0x962b9c62, posHash, true);
    posHash = FEHashUpper("POS2");
    FEngSetScript(GetPackageName(), 0xec85c7e4, posHash, true);
}

void PostRaceMilestonesScreen::StartAnimations(bool isMilestone, int typeKey, float bountyEarned, const char *descriptionStr) {
    mBountyEarned += bountyEarned;
    SetMilestoneAnimationScriptHash(isMilestone, typeKey);
    unsigned int iconHash = FEDatabase->GetMilestoneIconHash(typeKey, isMilestone);
    FEngSetTextureHash(mpDataBigIcon, iconHash);
    FEPrintf(GetPackageName(), 0xe526d0d2, "%s", descriptionStr);
    if (bountyEarned > 0.0f) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xe1045a4f));
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xe1045a4f));
    }
    const char *bountyStr = GetTranslatedString(0x29b1b96a);
    FEPrintf(GetPackageName(), 0xe1045a4f, "%s: %$0.0f", bountyStr, bountyEarned);
    const char *totalStr = GetTranslatedString(0x5ccf949a);
    FEPrintf(GetPackageName(), 0x324f7792, "%s: %$0.0f", totalStr, mBountyEarned);
    FEngSetScript(mpDataBigIcon, 0x5079c8f8, true);
}

bool PostRaceMilestonesScreen::StartBountyAnimations(bool copDestruction) {
    char buf[64];
    if (!copDestruction) {
        const char *str = GetTranslatedString(0x4d64888d);
        bSNPrintf(buf, 64, "%s", str);
        StartAnimations(false, 0x33fa23a, static_cast<float>(PostRacePursuitScreen::mPursuitData.mRepAchievedNormal), buf);
    } else {
        const char *str = GetTranslatedString(0x23f6e732);
        bSNPrintf(buf, 64, "%s: %$d", str, PostRacePursuitScreen::mPursuitData.mNumCopsDestroyed);
        StartAnimations(false, 0x4fc942ca, static_cast<float>(PostRacePursuitScreen::mPursuitData.mRepAchievedCopDestruction), buf);
    }
    return true;
}

bool PostRaceMilestonesScreen::SetMilestoneAnimationScriptHash(bool isMilestone, int type) {
    const char *posStr;
    if (type == 0x2377e50d) {
        posStr = "POS1";
    } else if (type == static_cast<int>(0xA61CAC24)) {
        posStr = "POS2";
    } else if (type == static_cast<int>(0xFD989A3A)) {
        posStr = "POS3";
    } else if (type == static_cast<int>(0xEB45F99D)) {
        posStr = "POS4";
    } else if (type == static_cast<int>(0xCDF36FC2)) {
        posStr = "POS5";
    } else if (type == static_cast<int>(0x850A64BC)) {
        posStr = "POS6";
    } else if (type == 0x33fa23a) {
        posStr = isMilestone ? "POS7" : "POS0";
    } else if (type == 0x5392e4fd) {
        posStr = "POS8";
    } else if (type == 0x4fc942ca) {
        posStr = "POS00";
    } else {
        mCurrMilestoneScriptHash = 0;
        return false;
    }
    mCurrMilestoneScriptHash = FEHashUpper(posStr);
    return mCurrMilestoneScriptHash != 0;
}

bool PostRaceMilestonesScreen::StartMilestoneAnimations() {
    mCurrMilestoneIndex++;
    const GMilestone *milestone = PostRacePursuitScreen::GetPursuitData().GetMilestone(mCurrMilestoneIndex);
    if (milestone) {
        char descStr[32];
        char outputStr[64];
        unsigned int typeKey = milestone->GetTypeKey();
        FEDatabase->SetMilestoneDescriptionString(descStr, 0, milestone->GetRequiredValue(), 0.0f, false);
        const char *header = GetLocalizedString(FEDatabase->GetMilestoneHeaderHash(typeKey));
        bSNPrintf(outputStr, 64, "%s: %s", header, descStr);
        StartAnimations(true, typeKey, milestone->GetBounty(), outputStr);
    } else {
        StartMilestoneDoneAnimations();
    }
    return milestone != nullptr;
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
            FEDatabase->SetMilestoneDescriptionString(descStr, 0, currVal, goalVal, false);
            const char *header = GetLocalizedString(FEDatabase->GetMilestoneHeaderHash(raceParams->GetChallengeType()));
            bSNPrintf(outputStr, 64, "%s: %s", header, descStr);
            StartAnimations(false, raceParams->GetChallengeType(), 0.0f, outputStr);
            return true;
        }
    }
    StartMilestoneDoneAnimations();
    return false;
}

extern bool FEngIsScriptRunning(FEObject *obj, unsigned int script_hash);

void PostRaceMilestonesScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x35f8620b) {
        StartBountyAnimations(false);
    } else if (msg < 0x35f8620bu) {
        if (msg == 0xd3c3de7) {
            if (!mCopDestructionBountyShown) {
                mCopDestructionBountyShown = true;
                if (PostRacePursuitScreen::mPursuitData.mNumCopsDestroyed > 0) {
                    StartBountyAnimations(true);
                    return;
                }
            }
            if (!GRaceStatus::Exists() || !GRaceStatus::Get().GetRaceParameters() ||
                !GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace() ||
                FEDatabase->IsFinalEpicChase()) {
                StartMilestoneAnimations();
            } else {
                StartChallengeAnimations();
            }
        }
    } else if (msg == 0x406415e3) {
        cFEng::mInstance->QueuePackagePop(1);
        new EShowResults(FERESULTTYPE_PURSUIT, false);
    } else if (msg == 0xc98356ba) {
        if (FEngIsScriptSet(mpDataBigIcon, 0x5079c8f8) &&
            !FEngIsScriptRunning(mpDataBigIcon, 0x5079c8f8)) {
            FEngSetScript(mpDataBigIcon, mCurrMilestoneScriptHash, true);
        }
    }
}

PursuitResultsDatum::PursuitResultsDatum(PursuitResultsDatumType type, unsigned int itemName, float itemNumber, float itemGoal, PursuitResultsDatumCheckType itemChecked)
    : ArrayDatum(0, 0) //
    , mType(type) //
    , mName(itemName) //
    , mNumber(itemNumber >= 0.0f ? itemNumber : 0.0f) //
    , mGoal(itemGoal) //
    , mChecked(itemChecked) //
{}

PursuitResultsArraySlot::PursuitResultsArraySlot(FEObject *obj, FEString *itemName, FEString *itemNumber, FEImage *itemChecked, FEImage *itemEmpty)
    : ArraySlot(obj) //
    , mLine(obj) //
    , mItemName(itemName) //
    , mItemNumber(itemNumber) //
    , mItemChecked(itemChecked) //
    , mItemEmpty(itemEmpty) //
{}

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
        FEngSetLanguageHash(mItemName, data->mName);
    }

    PursuitResultsDatum::PursuitResultsDatumType type = data->mType;

    if (type == PursuitResultsDatum::PursuitResultsDatumType_Number) {
        FEPrintf(mItemNumber, lbl_803E4CB4, static_cast<int>(data->mNumber));
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_On) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_Greyed) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Time) {
        Timer timer;
        timer.SetTime(data->mNumber);
        char text[32];
        timer.PrintToString(text, 0);
        FEPrintf(mItemNumber, lbl_803E4CF0, text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_On) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_Greyed) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Number) {
        char text[32];
        FEDatabase->SetMilestoneDescriptionString(text, -1, data->mNumber, data->mGoal, false);
        FEPrintf(mItemNumber, lbl_803E4CF0, text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_On) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_Greyed) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time) {
        char text[32];
        FEDatabase->SetMilestoneDescriptionString(text, -1, data->mNumber, data->mGoal, true);
        FEPrintf(mItemNumber, lbl_803E4CF0, text);
        FEngSetScript(mItemNumber, 0x1744B3, true);
        if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_On) {
            FEngSetScript(mItemChecked, 0x1CA7C0, true);
        } else if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_Greyed) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x16A259, true);
        }
    } else if (type == PursuitResultsDatum::PursuitResultsDatumType_Milestone_Time_PursuitRemaining) {
        FEngSetScript(mItemNumber, 0x16A259, true);
        if (data->mChecked == PursuitResultsDatum::PursuitResultsDatumCheckType_On) {
            FEngSetScript(mItemChecked, 0x163C76, true);
        } else {
            FEngSetScript(mItemChecked, 0x163C76, true);
        }
    }
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

void PostRacePursuitScreen::SetupPursuit() {
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Time, 0x4d64888d, mPursuitData.mPursuitLength, 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xa999f6e2, static_cast<float>(mPursuitData.mNumCopsDamaged), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x23f6e732, static_cast<float>(mPursuitData.mNumCopsDestroyed), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x0291c816, static_cast<float>(mPursuitData.mNumSpikeStripsDodged), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0x2df2ba15, static_cast<float>(mPursuitData.mNumRoadblocksDodged), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xd9bb7d2d, static_cast<float>(mPursuitData.mCostToStateAchieved), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
    AddDatum(new PursuitResultsDatum(PursuitResultsDatum::PursuitResultsDatumType_Number, 0xb7dfff96, static_cast<float>(GInfractionManager::Get().GetNumInfractions()), 0.0f, PursuitResultsDatum::PursuitResultsDatumCheckType_Off));
}
