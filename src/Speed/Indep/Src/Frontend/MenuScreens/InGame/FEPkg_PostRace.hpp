#ifndef FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H
#define FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct GRacerInfo;

typedef int dialog_handle;

enum PostRaceScreenMode {
    POSTRACESCREENMODE_RESULTS = 0,
    POSTRACESCREENMODE_STATS = 1,
    POSTRACESCREENMODE_LAPSTATS = 2,
    POSTRACESCREENMODE_NUMMODES = 3,
};

struct PursuitData {
    void ClearData();

    bool mPursuitIsActive;
};

struct PostRacePursuitScreen {
    static PursuitData mPursuitData;

    static PursuitData &GetPursuitData() {
        return mPursuitData;
    }
};

struct RaceStat : public FEStatWidget {
    RaceStat(FEString *title, FEString *data);
    ~RaceStat() override;
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

#endif
