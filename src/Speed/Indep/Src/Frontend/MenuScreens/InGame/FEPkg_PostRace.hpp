#ifndef FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H
#define FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct GRacerInfo;
struct GMilestone;

typedef int dialog_handle;

enum PostRaceScreenMode {
    POSTRACESCREENMODE_RESULTS = 0,
    POSTRACESCREENMODE_STATS = 1,
    POSTRACESCREENMODE_LAPSTATS = 2,
    POSTRACESCREENMODE_NUMMODES = 3,
};

// total size: 0xAC
struct PursuitData {
    void ClearData();
    void PopulateData(struct IPursuit *ipursuit, struct IPerpetrator *iperpetrator, int exitToSafehouse);
    bool AddMilestone(GMilestone *milestone);
    const GMilestone *const GetMilestone(int index) const;
    int GetNumMilestones() { return mNumMilestonesThisPursuit; }

    static const int mMaxNumMilestones;

    bool mPursuitIsActive;             // offset 0x0
    float mPursuitLength;              // offset 0x4
    int mNumCopsDamaged;               // offset 0x8
    int mNumCopsDestroyed;             // offset 0xC
    int mNumSpikeStripsDodged;         // offset 0x10
    int mNumRoadblocksDodged;          // offset 0x14
    int mCostToStateAchieved;          // offset 0x18
    int mRepAchievedNormal;            // offset 0x1C
    int mRepAchievedCopDestruction;    // offset 0x20
    int mExitToSafehouse;              // offset 0x24
    int mNumMilestonesThisPursuit;     // offset 0x28
    GMilestone *mMilestonesCompleted[32]; // offset 0x2C
};

struct PostRacePursuitScreen {
    static PursuitData mPursuitData;

    static PursuitData &GetPursuitData() {
        return mPursuitData;
    }
};

struct RaceStat : public FEStatWidget {
    RaceStat(FEString *title, FEString *data);
    ~RaceStat() override {}
};

struct ResultStat : public RaceStat {
    ResultStat(FEString *name_str, FEString *data, FEString *pos, GRacerInfo *racer_info)
        : RaceStat(name_str, data) //
        , Position(pos) //
        , RacerInfo(racer_info) {}

    ~ResultStat() override {}

    FEString *Position;
    GRacerInfo *RacerInfo;
};

struct RaceResultStat : public ResultStat {
    RaceResultStat(FEString *name_str, FEString *time, FEString *pos, GRacerInfo *info)
        : ResultStat(name_str, time, pos, info) {}

    ~RaceResultStat() override;
    void Draw() override;
};

struct StageStat : public ResultStat {
    StageStat(FEString *stage, FEString *time, FEString *pos, int stage_num, float seconds, int pos_num)
        : ResultStat(stage, time, pos, nullptr) //
        , StageNum(stage_num) //
        , Time(seconds) //
        , PosNum(pos_num) {}

    ~StageStat() override;
    void Draw() override;

    int StageNum;
    Timer Time;
    int PosNum;
};

struct TollboothStat : public ResultStat {
    TollboothStat(FEString *tollbooth, FEString *time, FEString *pos, int tollbooth_num, float seconds, int pos_num)
        : ResultStat(tollbooth, time, pos, nullptr) //
        , TollboothNum(tollbooth_num) //
        , Time(seconds) //
        , PosNum(pos_num) {}

    ~TollboothStat() override;
    void Draw() override;

    int TollboothNum;
    Timer Time;
    int PosNum;
};

struct StatsPanel {
    StatsPanel();
    virtual ~StatsPanel();

    void SetParentPkg(const char *parent_pkg) {
        ParentPkg = parent_pkg;
    }

    void SetRacerName(const char *name) {
        RacerName = name;
    }

    FEString *GetCurrentString(const char *name);
    int GetNumStats() {
        return TheStats.CountElements();
    }

    void Reset();
    void Draw(unsigned int numPlayers);
    void AddStat(RaceStat *stat);
    void AddInfoStat(unsigned int title, unsigned int info);
    void AddGenericStat(float stat_data, unsigned int title_hash, unsigned int units_hash, const char *format);
    void AddTimerStat(float seconds, unsigned int title_hash);

    bTList<FEWidget> TheStats;
    int iWidgetToAdd;
    const char *RacerName;
    const char *ParentPkg;
};

struct PostRaceResultsScreen : public MenuScreen {
  public:
    PostRaceResultsScreen(ScreenConstructorData *sd);
    ~PostRaceResultsScreen() override;
    void NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long Param1, unsigned long Param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;

  private:
    StatsPanel &GetActiveStatsPanel() {
        if (mPostRaceScreenMode != POSTRACESCREENMODE_RESULTS && mIndexOfCurrentRacer >= 0 && mIndexOfCurrentRacer < 16) {
            return RacerStats[mIndexOfCurrentRacer];
        }

        return RaceResults;
    }

    void Setup();
    void SetupResults();
    void SetupStat_NosUsed();
    void SetupStat_TopSpeed();
    void SetupStat_AverageSpeed();
    void SetupStat_TimeBehind();
    void SetupStat_LapVariance();
    void SetupStat_StageVariance();
    void SetupStat_TrafficCollisions();
    void SetupStat_ZeroToSixty();
    void SetupStat_QuarterMile();
    void SetupStat_PerfectShifts();
    void SetupStat_AccumulatedSpeed();
    void SetupStat_SpeedVariance();
    void SetupStat_SpeedBehind();
    void SetupRacerStats(int index, GRacerInfo *racer_info);
    void SetupLapStats(int racerIndex, GRacerInfo *racer_info);

    StatsPanel RacerStats[16];
    StatsPanel RaceResults;
    // TODO: PS2 type info shows a dialog_handle at 0x1C4, but GC ctor/diffs clearly use
    // this GameCube layout instead. Reconcile once NotificationMessage/upload paths are in.
    int mNumberOfRacers;
    int mIndexOfWinner;
    int mIndexOfCurrentRacer;
    int mNumberOfLaps;
    int mNumberOfStats;
    GRace::Type mRaceType;
    PostRaceScreenMode mPostRaceScreenMode;
    GRacerInfo *mPlayerRacerInfo;
    int mMaxSlotsLeftSide;
    unsigned int m_RaceButtonHash;
    int m_lastErrorKind;
    int m_lastErrorCode;
    bool m_raceResultsUploaded;
};

// total size: 0x38
struct PursuitResultsDatum : public ArrayDatum {
    enum PursuitResultsDatumType {
        PursuitResultsDatumType_Number = 0,
        PursuitResultsDatumType_Time = 1,
        PursuitResultsDatumType_Milestone_Number = 2,
        PursuitResultsDatumType_Milestone_Time = 3,
        PursuitResultsDatumType_Milestone_Time_PursuitRemaining = 4,
        PursuitResultsDatumType_Check = 5,
    };
    enum PursuitResultsDatumCheckType {
        PursuitResultsDatumCheckType_Off = 0,
        PursuitResultsDatumCheckType_On = 1,
        PursuitResultsDatumCheckType_Greyed = 2,
    };

    PursuitResultsDatum(PursuitResultsDatumType type, unsigned int headerHash, int value, float fvalue, PursuitResultsDatumCheckType checkType);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override {}
};

#endif
