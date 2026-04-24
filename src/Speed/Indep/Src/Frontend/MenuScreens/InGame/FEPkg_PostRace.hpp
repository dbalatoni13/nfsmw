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

enum PostPursuitScreenMode {
    POSTPURSUITSCREENMODE_PURSUIT = 0,
    POSTPURSUITSCREENMODE_INFRACTIONS = 1,
    POSTPURSUITSCREENMODE_MILESTONES = 2,
};

struct PostRacePursuitScreen : ArrayScrollerMenu {
    PostRacePursuitScreen(ScreenConstructorData *);
    ~PostRacePursuitScreen() override;
    void Initialize();
    void SetupInfractions();
    void SetupMilestones();
    void SetupPursuit();
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    static PursuitData mPursuitData;

    static PursuitData &GetPursuitData() {
        return mPursuitData;
    }

    PostPursuitScreenMode mPostPursuitScreenMode; // offset 0xE8
    unsigned int m_RaceButtonHash;                 // offset 0xEC
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

struct GenericStat : public RaceStat {
    GenericStat(FEString *title, FEString *data, float stat_data, unsigned int title_hash,
                unsigned int units_hash, const char *format)
        : RaceStat(title, data) //
        , StatData(stat_data) //
        , TitleHash(title_hash) //
        , UnitsHash(units_hash) //
        , Format(format) {}

    ~GenericStat() override {}
    void Draw() override;

    float StatData;
    unsigned int TitleHash;
    unsigned int UnitsHash;
    const char *Format;
};

struct InfoStat : public RaceStat {
    InfoStat(FEString *title, FEString *info, unsigned int title_hash, unsigned int info_hash)
        : RaceStat(title, info) //
        , TitleHash(title_hash) //
        , InfoHash(info_hash) {}

    ~InfoStat() override {}
    void Draw() override;

    unsigned int TitleHash;
    unsigned int InfoHash;
};

struct TimerStat : public RaceStat {
    TimerStat(FEString *title, FEString *data, float seconds, unsigned int title_hash)
        : RaceStat(title, data) //
        , Seconds(seconds) //
        , TitleHash(title_hash) {}

    ~TimerStat() override {}
    void Draw() override;

    Timer Seconds;
    unsigned int TitleHash;
};

struct GenericResult : public ResultStat {
    GenericResult(FEString *name, FEString *data, FEString *pos, unsigned int units_hash,
                  float fdata, const char *format, GRacerInfo *racer_info)
        : ResultStat(name, data, pos, racer_info) //
        , UnitsHash(units_hash) //
        , FData(fdata) //
        , Format(format) {}

    ~GenericResult() override {}
    void Draw() override;

    unsigned int UnitsHash;
    float FData;
    const char *Format;
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
    virtual ~StatsPanel() {}

    void SetParentPkg(const char *parent_pkg) {
        ParentPkg = parent_pkg;
    }

    void SetRacerName(const char *name) {
        RacerName = name;
    }

    FEString *GetCurrentString(const char *name) {
        extern const char lbl_803E5088[];

        return FEngFindString(ParentPkg, FEngHashString(lbl_803E5088, name, iWidgetToAdd));
    }
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

    PursuitResultsDatum(PursuitResultsDatumType type, unsigned int itemName, float itemNumber, float itemGoal, PursuitResultsDatumCheckType itemChecked);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override {}

    PursuitResultsDatumType GetType() { return mType; }
    unsigned int GetName() { return mName; }
    float GetNumber() { return mNumber; }
    float GetGoal() { return mGoal; }
    bool GetChecked() { return mChecked == PursuitResultsDatumCheckType_On; }
    bool GetGreyed() { return mChecked == PursuitResultsDatumCheckType_Greyed; }

    PursuitResultsDatumType mType;           // offset 0x24, size 0x4
    unsigned int mName;                       // offset 0x28, size 0x4
    float mNumber;                            // offset 0x2C, size 0x4
    float mGoal;                              // offset 0x30, size 0x4
    PursuitResultsDatumCheckType mChecked;    // offset 0x34, size 0x4
};

struct PursuitResultsArraySlot : public ArraySlot {
    PursuitResultsArraySlot(FEObject *obj, FEString *itemName, FEString *itemNumber, FEImage *itemChecked, FEImage *itemEmpty);
    ~PursuitResultsArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

    FEObject *mLine;          // offset 0x14, size 0x4
    FEString *mItemName;      // offset 0x18, size 0x4
    FEString *mItemNumber;    // offset 0x1C, size 0x4
    FEImage *mItemChecked;    // offset 0x20, size 0x4
    FEImage *mItemEmpty;      // offset 0x24, size 0x4
};

struct PostRaceMilestonesScreen : MenuScreen {
    PostRaceMilestonesScreen(ScreenConstructorData *);
    ~PostRaceMilestonesScreen() override;
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    void StartMilestoneDoneAnimations();
    void StartAnimations(bool isMilestone, int typeKey, float bountyEarned, const char *descriptionStr);
    bool StartBountyAnimations(bool copDestruction);
    bool SetMilestoneAnimationScriptHash(bool isMilestone, int type);
    bool StartMilestoneAnimations();
    bool StartChallengeAnimations();
    FEImage *mpDataBigIcon;          // offset 0x2C
    float mBountyEarned;             // offset 0x30
    bool mCopDestructionBountyShown; // offset 0x34
    int mCurrMilestoneIndex;         // offset 0x38
    int mCurrMilestoneScriptHash;    // offset 0x3C
};

#endif
