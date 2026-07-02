#ifndef __FEPKG_POSTRACE_H__
#define __FEPKG_POSTRACE_H__

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

typedef enum {
    POSTRACESCREENMODE_RESULTS = 0,
    POSTRACESCREENMODE_STATS = 1,
    POSTRACESCREENMODE_LAPSTATS = 2,
    POSTRACESCREENMODE_NUMMODES = 3
} PostRaceScreenMode;

typedef enum { POSTPURSUITSCREENMODE_PURSUIT = 0, POSTPURSUITSCREENMODE_INFRACTIONS = 1, POSTPURSUITSCREENMODE_MILESTONES = 2 } PostPursuitScreenMode;

class RaceStat : public FEStatWidget {
  public:
    RaceStat(FEString *title, FEString *data);
    ~RaceStat() override {}
};

class InfoStat : public RaceStat {
  public:
    InfoStat(FEString *title, FEString *info, uint32 title_hash, uint32 info_hash) : RaceStat(title, info) {
        TitleHash = title_hash;
        InfoHash = info_hash;
    }

    ~InfoStat() override {}
    void Draw() override {
        FEngSetLanguageHash(GetTitleObject(), TitleHash);
        FEngSetLanguageHash(GetDataObject(), InfoHash);
    };

  protected:
    uint32 TitleHash;
    uint32 InfoHash;
};

class GenericStat : public RaceStat {
  public:
    GenericStat(FEString *title, FEString *data, float stat_data, uint32 title_hash, uint32 units_hash, const char *format) : RaceStat(title, data) {
        StatData = stat_data;
        TitleHash = title_hash;
        UnitsHash = units_hash;
        Format = format;
    }

    ~GenericStat() override {}
    void Draw() override {
        char text[0x20];
        FEngSetLanguageHash(GetTitleObject(), TitleHash);
        bSNPrintf(text, 0x20, Format, StatData);
        if (UnitsHash != 0) {
            FEString *data = GetDataObject();
            const char *units = GetLocalizedString(UnitsHash);
            FEPrintf(data, "%s %s", text, units);
        } else {
            FEPrintf(GetDataObject(), "%s", text);
        }
    };

  protected:
    float StatData;
    uint32 TitleHash;
    uint32 UnitsHash;
    const char *Format;
};

class TimerStat : public RaceStat {
  public:
    TimerStat(FEString *title, FEString *data, float seconds, uint32 title_hash) : RaceStat(title, data) {
        Seconds = seconds;
        TitleHash = title_hash;
    }

    ~TimerStat() override {}
    void Draw() override {
        char text[0x20];
        FEngSetLanguageHash(GetTitleObject(), TitleHash);
        Seconds.PrintToString(text, 0);
        FEPrintf(GetDataObject(), "%s", text);
    };

  protected:
    Timer Seconds;
    uint32 TitleHash;
};

class ResultStat : public RaceStat {
  public:
    ResultStat(FEString *name_str, FEString *data, FEString *pos, GRacerInfo *racer_info) : RaceStat(name_str, data) {
        Position = pos;
        RacerInfo = racer_info;
    }
    ~ResultStat() override {}

  protected:
    FEString *Position;
    GRacerInfo *RacerInfo;
};

class GenericResult : public ResultStat {
  public:
    GenericResult(FEString *name, FEString *data, FEString *pos, uint32 units_hash, float fdata, const char *format, GRacerInfo *racer_info)
        : ResultStat(name, data, pos, racer_info) {
        UnitsHash = units_hash;
        FData = fdata;
        Format = format;
    }

    ~GenericResult() override {}
    void Draw() override {
        char text[0x20];

        FEPrintf(GetTitleObject(), "%s", RacerInfo->GetName());

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
            FEPrintf(GetDataObject(), "%s", text);
        }

    show_position:
        FEPrintf(Position, "%d", RacerInfo->GetRanking());
    };

  protected:
    uint32 UnitsHash;
    float FData;
    const char *Format;
};

class RaceResultStat : public ResultStat {
  public:
    RaceResultStat(FEString *name_str, FEString *time, FEString *pos, GRacerInfo *info) : ResultStat(name_str, time, pos, info) {}

    ~RaceResultStat() override;
    void Draw() override {
        char text[0x20];

        FEPrintf(GetTitleObject(), "%s", RacerInfo->GetName());

        if (RacerInfo->GetIsEngineBlown()) {
            FEngSetLanguageHash(GetDataObject(), 0x01E66364);
        } else if (RacerInfo->GetIsTotalled()) {
            FEngSetLanguageHash(GetDataObject(), 0xB7B75185);
        } else if (RacerInfo->GetIsKnockedOut()) {
            FEngSetLanguageHash(GetDataObject(), 0x5D82DBA2);
        } else if (!RacerInfo->IsFinishedRacing()) {
            FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
        } else {
            Timer t(RacerInfo->GetRaceTime());
            t.PrintToString(text, 0);
            FEPrintf(GetDataObject(), "%s", text);
        }

        FEPrintf(Position, "%d", RacerInfo->GetRanking());
    };
};

class LapStat : public ResultStat {
  public:
    LapStat(FEString *lap, FEString *time, FEString *pos, int lap_num, int seconds, int pos_num) : ResultStat(lap, time, pos, nullptr) {
        LapNum = lap_num;
        Time = seconds;
        PosNum = pos_num;
    };

    ~LapStat() override {};
    void Draw() override {
        FEPrintf(GetTitleObject(), "%d", LapNum);
        if (static_cast<float>(PosNum) == 0.0f) {
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
        FEPrintf(Position, "%d", PosNum);
        Time.PrintToString(text, 0);
        FEPrintf(GetDataObject(), "%s", text);
    };

  protected:
    int LapNum;
    Timer Time;
    int PosNum;
};

class StageStat : public ResultStat {
  public:
    StageStat(FEString *stage, FEString *time, FEString *pos, int stage_num, float seconds, int pos_num) : ResultStat(stage, time, pos, nullptr) {
        StageNum = stage_num;
        Time = seconds;
        PosNum = pos_num;
    }

    ~StageStat() override {};
    void Draw() override {
        FEPrintf(GetTitleObject(), "%d%s", (StageNum + 1) * 0x14, GetLocalizedPercentSign());
        if (Time.GetSeconds() == 0.0f) {
            FEngSetLanguageHash(Position, 0x0FC1BF40);
            FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
            return;
        }

        char text[32];
        FEPrintf(Position, "%d", PosNum);
        Time.PrintToString(text, 0);
        FEPrintf(GetDataObject(), "%s", text);
    };

  protected:
    int StageNum;
    Timer Time;
    int PosNum;
};

class SpeedStat : public ResultStat {
  public:
    SpeedStat(FEString *lap, FEString *speedStr, FEString *pos, int lap_num, float speed, int pos_num) : ResultStat(lap, speedStr, pos, nullptr) {
        LapNum = lap_num;
        Speed = speed;
        PosNum = pos_num;
    };

    ~SpeedStat() override {};
    void Draw() override {
        FEPrintf(GetTitleObject(), "%d", LapNum);
        if (Speed == 0.0f) {
            FEngSetLanguageHash(Position, 0x0FC1BF40);
            FEngSetLanguageHash(GetDataObject(), 0x0FC1BF40);
            return;
        }

        float scale = 2.23699f;
        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            scale = 3.6f;
        }

        FEPrintf(Position, "%d", PosNum);
        FEPrintf(GetDataObject(), "%0.0f", Speed * scale);
    };

  protected:
    int LapNum;
    float Speed;
    int PosNum;
};

class TollboothStat : public ResultStat {
  public:
    TollboothStat(FEString *tollbooth, FEString *time, FEString *pos, int tollbooth_num, float seconds, int pos_num)
        : ResultStat(tollbooth, time, pos, nullptr) {
        TollboothNum = tollbooth_num;
        Time = seconds;
        PosNum = pos_num;
    }

    ~TollboothStat() override {};
    void Draw() override {
        FEPrintf(GetTitleObject(), "%d", TollboothNum);
        if (static_cast<float>(PosNum) == 1.0f) {
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
        FEPrintf(Position, "%d", PosNum);
        Time.PrintToString(text, 0);
        FEPrintf(GetDataObject(), "+%s", text);
    };

  protected:
    int TollboothNum;
    Timer Time;
    int PosNum;
};

class StatsPanel {
  public:
    StatsPanel();
    virtual ~StatsPanel() {}

    void Reset();
    void Draw(uint32 numPlayers);

    void SetParentPkg(const char *parent_pkg) {
        ParentPkg = parent_pkg;
    }

    void SetRacerName(const char *name) {
        RacerName = name;
    }

    void AddStat(RaceStat *stat);
    void AddInfoStat(uint32 title, uint32 info);
    void AddGenericStat(float stat_data, uint32 title_hash, uint32 units_hash, const char *format);
    void AddTimerStat(float seconds, uint32 title_hash);

    FEString *GetCurrentString(const char *name) {
        return FEngFindString(ParentPkg, FEngHashString("%s%d", name, iWidgetToAdd));
    }

    int GetNumStats() {
        return TheStats.CountElements();
    }

  protected:
    bTList<FEWidget> TheStats;
    int iWidgetToAdd;
    const char *RacerName;
    const char *ParentPkg;
};

class PostRaceResultsScreen : public MenuScreen {
  public:
    PostRaceResultsScreen(ScreenConstructorData *sd);
    ~PostRaceResultsScreen() override;
    void NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
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
// TODO are the following gated by an online enable define?
#if 0
    static void ResultUpdateCB(void *context);
    void UploadSavedRaceResults();
    // TODO
    // static void SendRaceResultsCB(LobbyApiMsgT *msg, void *context);
#endif

    StatsPanel RacerStats[16];
    StatsPanel RaceResults;
#if EA_BUILD_A124
    dialog_handle msgHandle;
#endif
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

// total size: 0xAC
class PursuitData {
  public:
    void PopulateData(IPursuit *ipursuit, IPerpetrator *iperpetrator, int exitToSafehouse);
    void ClearData();
    bool AddMilestone(GMilestone *milestone);
    const GMilestone *const GetMilestone(int index) const;
    int GetNumMilestones() {
        return mNumMilestonesThisPursuit;
    }

    bool mPursuitIsActive;          // offset 0x0
    float mPursuitLength;           // offset 0x4
    int mNumCopsDamaged;            // offset 0x8
    int mNumCopsDestroyed;          // offset 0xC
    int mNumSpikeStripsDodged;      // offset 0x10
    int mNumRoadblocksDodged;       // offset 0x14
    int mCostToStateAchieved;       // offset 0x18
    int mRepAchievedNormal;         // offset 0x1C
    int mRepAchievedCopDestruction; // offset 0x20
    int mExitToSafehouse;           // offset 0x24
  private:
    static const int mMaxNumMilestones;
    int mNumMilestonesThisPursuit;        // offset 0x28
    GMilestone *mMilestonesCompleted[32]; // offset 0x2C
};

// total size: 0x38
class PursuitResultsDatum : public ArrayDatum {
  public:
    typedef enum {
        PursuitResultsDatumType_Number = 0,
        PursuitResultsDatumType_Time = 1,
        PursuitResultsDatumType_Milestone_Number = 2,
        PursuitResultsDatumType_Milestone_Time = 3,
        PursuitResultsDatumType_Check = 4
    } PursuitResultsDatumType;

    typedef enum {
        PursuitResultsDatumCheckType_Off = 0,
        PursuitResultsDatumCheckType_On = 1,
        PursuitResultsDatumCheckType_Greyed = 2,
    } PursuitResultsDatumCheckType;

    PursuitResultsDatum(PursuitResultsDatumType type, uint32 itemName, float itemNumber, float itemGoal, PursuitResultsDatumCheckType itemChecked);
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    PursuitResultsDatumType GetType() {
        return mType;
    }
    uint32 GetName() {
        return mName;
    }
    float GetNumber() {
        return mNumber;
    }
    float GetGoal() {
        return mGoal;
    }
    bool GetChecked() {
        return mChecked == PursuitResultsDatumCheckType_On;
    }
    bool GetGreyed() {
        return mChecked == PursuitResultsDatumCheckType_Greyed;
    }

  private:
    PursuitResultsDatumType mType;         // offset 0x24, size 0x4
    uint32 mName;                          // offset 0x28, size 0x4
    float mNumber;                         // offset 0x2C, size 0x4
    float mGoal;                           // offset 0x30, size 0x4
    PursuitResultsDatumCheckType mChecked; // offset 0x34, size 0x4
};

class PursuitResultsArraySlot : public ArraySlot {
  public:
    PursuitResultsArraySlot(FEObject *obj, FEString *itemName, FEString *itemNumber, FEImage *itemChecked, FEImage *itemEmpty);
    ~PursuitResultsArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  private:
    FEObject *mLine;       // offset 0x14, size 0x4
    FEString *mItemName;   // offset 0x18, size 0x4
    FEString *mItemNumber; // offset 0x1C, size 0x4
    FEImage *mItemChecked; // offset 0x20, size 0x4
    FEImage *mItemEmpty;   // offset 0x24, size 0x4
};

class PostRacePursuitScreen : public ArrayScrollerMenu {
  public:
    PostRacePursuitScreen(ScreenConstructorData *);
    ~PostRacePursuitScreen() override;
    void NotificationMessage(u32 msg, struct FEObject *pobj, u32 param1, u32 param2) override;
    static PursuitData &GetPursuitData() {
        return mPursuitData;
    }

  private:
    void Initialize();
    void SetupInfractions();
    void SetupMilestones();
    void SetupPursuit();

    PostPursuitScreenMode mPostPursuitScreenMode; // offset 0xE8
    uint32 m_RaceButtonHash;                      // offset 0xEC
    static PursuitData mPursuitData;
};

class PostRaceMilestonesScreen : public MenuScreen {
  public:
    PostRaceMilestonesScreen(ScreenConstructorData *);
    ~PostRaceMilestonesScreen() override;
    void NotificationMessage(u32 msg, struct FEObject *pobj, u32 param1, u32 param2) override;

  private:
    bool StartMilestoneAnimations();
    bool StartChallengeAnimations();
    bool StartBountyAnimations(bool copDestruction);
    void StartAnimations(bool isMilestone, int typeKey, float bountyEarned, const char *descriptionStr);
    void StartMilestoneDoneAnimations();
    bool SetMilestoneAnimationScriptHash(bool isMilestone, int type);

    FEImage *mpDataBigIcon;          // offset 0x2C
    float mBountyEarned;             // offset 0x30
    bool mCopDestructionBountyShown; // offset 0x34
    int mCurrMilestoneIndex;         // offset 0x38
    int mCurrMilestoneScriptHash;    // offset 0x3C
};

#endif
