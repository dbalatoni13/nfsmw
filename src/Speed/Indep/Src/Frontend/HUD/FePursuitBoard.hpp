#ifndef FEPURSUITBOARD_H
#define FEPURSUITBOARD_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

class PursuitBoard : public HudElement, public IPursuitBoard {
  public:
    PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~PursuitBoard() override {}

    void Update(IPlayer *player) override;
    void SetInPursuit(bool inPursuit) override;
    void SetIsHiding(bool isHiding) override;
    void SetTimeUntilHidden(float time) override;
    void SetTimeUntilBusted(float time, bool bIsBusted) override;
    void SetTimeUntilBackup(float time) override;
    void SetIsInView(bool isInView) override;
    void SetPursuitDuration(float time) override;
    void SetCooldownTimeRemaining(float time) override;
    void SetCooldownTimeRequired(float time) override;
    void SetNumCopsInPursuit(int numCops) override;
    void SetNumCopsDestroyed(int numCops, UCrc32 lastCopDestroyedType, int lastCopDestroyedMultiplier, int lastCopDestroyedRep) override;
    void SetNumCopsDamaged(int numCops) override;
    void SetTotalNumCopsInvolved(int numCops) override;
    void SetHeliInvolvedInPursuit(bool heliInvolved) override;
    void SetPursuitRep(int rep) override;

  private:
    bool mInPursuit;                          // offset 0x30, size 0x1
    bool mIsHiding;                           // offset 0x34, size 0x1
    float mTimeUntilHidden;                   // offset 0x38, size 0x4
    float mTimeUntilBusted;                   // offset 0x3C, size 0x4
    float mTimeUntilBackup;                   // offset 0x40, size 0x4
    bool mIsInView;                           // offset 0x44, size 0x1
    float mPursuitDuration;                   // offset 0x48, size 0x4
    float mCooldownTimeRemaining;             // offset 0x4C, size 0x4
    float mCooldownTimeRequired;              // offset 0x50, size 0x4
    int mNumCopsFullyEngaged;                 // offset 0x54, size 0x4
    int mNumCopsDestroyed;                    // offset 0x58, size 0x4
    int mNumCopsDamaged;                      // offset 0x5C, size 0x4
    int mTotalNumCopsInvolved;                // offset 0x60, size 0x4
    bool mHeliInvolved;                       // offset 0x64, size 0x1
    int mPursuitRep;                          // offset 0x68, size 0x4
    FEGroup *mpDataPursuitBoardGroup;         // offset 0x6C, size 0x4
    FEGroup *mpDataPursuitMeterGroup;         // offset 0x70, size 0x4
    FEGroup *mpDataPursuitIconsGroup;         // offset 0x74, size 0x4
    FEGroup *mpDataPursuitSummaryGroup;       // offset 0x78, size 0x4
    FEGroup *mpDataPursuitCooldownMeterGroup; // offset 0x7C, size 0x4
    FEGroup *mpDataBackupTimerTextGroup;      // offset 0x80, size 0x4
    FEString *mpDataPursuitTimer;             // offset 0x84, size 0x4
    FEString *mpDataBackupTimer;              // offset 0x88, size 0x4
    FEString *mpDataPursuitCopsNumbers;       // offset 0x8C, size 0x4
    FEString *mpDataCopsTakenOut;             // offset 0x90, size 0x4
    FEString *mpDataCopsDamaged;              // offset 0x94, size 0x4
    FEString *mpDataPursuitSummaryTotal;      // offset 0x98, size 0x4
    FEObject *mpDataBustedBar0;               // offset 0x9C, size 0x4
    FEObject *mpDataBustedBar1;               // offset 0xA0, size 0x4
    FEObject *mpDataBustedBar2;               // offset 0xA4, size 0x4
    FEObject *mpDataBustedBar3;               // offset 0xA8, size 0x4
    FEObject *mpDataBustedBar4;               // offset 0xAC, size 0x4
    FEObject *mpDataCooldownBar;              // offset 0xB0, size 0x4
    FEObject *mpDataBackupBacking;            // offset 0xB4, size 0x4
    FEObject *mpDataHidingBacking;            // offset 0xB8, size 0x4
    float mBustedBarOriginalWidth0;           // offset 0xBC, size 0x4
    float mBustedBarOriginalWidth1;           // offset 0xC0, size 0x4
    float mBustedBarOriginalWidth2;           // offset 0xC4, size 0x4
    float mBustedBarOriginalWidth3;           // offset 0xC8, size 0x4
    float mBustedBarOriginalWidth4;           // offset 0xCC, size 0x4
    float mCooldownBarOriginalWidth;          // offset 0xD0, size 0x4
};

#endif
