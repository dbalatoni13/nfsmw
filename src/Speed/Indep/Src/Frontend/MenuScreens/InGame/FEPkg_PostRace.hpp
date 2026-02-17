#ifndef FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H
#define FRONTEND_MENUSCREENS_INGAME_FEPKG_POSTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

enum PostPursuitScreenMode {
    POSTPURSUITSCREENMODE_PURSUIT = 0,
    POSTPURSUITSCREENMODE_INFRACTIONS = 1,
    POSTPURSUITSCREENMODE_MILESTONES = 2,
};

// TODO MOVE PursuitData, idk where

// total size: 0xAC
class PursuitData {
  public:
    PursuitData() {}

    // int GetNumMilestones() {}

    // void PopulateData(struct IPursuit *ipursuit, struct IPerpetrator *iperpetrator, int exitToSafehouse);

    // bool AddMilestone(struct GMilestone *milestone);

    // const struct GMilestone *const GetMilestone(const int index) const;

    void ClearData();

  private:
    static const int mMaxNumMilestones; // size: 0x4, address: 0xFFFFFFFF

    bool mPursuitIsActive;                       // offset 0x0, size 0x1
    float mPursuitLength;                        // offset 0x4, size 0x4
    int mNumCopsDamaged;                         // offset 0x8, size 0x4
    int mNumCopsDestroyed;                       // offset 0xC, size 0x4
    int mNumSpikeStripsDodged;                   // offset 0x10, size 0x4
    int mNumRoadblocksDodged;                    // offset 0x14, size 0x4
    int mCostToStateAchieved;                    // offset 0x18, size 0x4
    int mRepAchievedNormal;                      // offset 0x1C, size 0x4
    int mRepAchievedCopDestruction;              // offset 0x20, size 0x4
    int mExitToSafehouse;                        // offset 0x24, size 0x4
    int mNumMilestonesThisPursuit;               // offset 0x28, size 0x4
    struct GMilestone *mMilestonesCompleted[32]; // offset 0x2C, size 0x80
};

// total size: 0xF0
class PostRacePursuitScreen : public ArrayScrollerMenu {
  public:
    static PursuitData &GetPursuitData() {
        return mPursuitData;
    }

    PostRacePursuitScreen(ScreenConstructorData *sd);

    // Overrides: MenuScreen
    ~PostRacePursuitScreen() override;

    void Initialize();

    void SetupInfractions();

    void SetupMilestones();

    void SetupPursuit();

    // Overrides: MenuScreen
    void NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) override;

  private:
    static PursuitData mPursuitData; // size: 0xAC, address: 0x8047306C

    PostPursuitScreenMode mPostPursuitScreenMode; // offset 0xE8, size 0x4
    unsigned int m_RaceButtonHash;                // offset 0xEC, size 0x4
};

#endif
