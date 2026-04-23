#ifndef FRONTEND_HUD_FEPURSUITBOARD_H
#define FRONTEND_HUD_FEPURSUITBOARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

class PursuitBoard : public HudElement, public IPursuitBoard {
  public:
    PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
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
    bool mInPursuit;
    bool mIsHiding;
    float mTimeUntilHidden;
    float mTimeUntilBusted;
    float mTimeUntilBackup;
    bool mIsInView;
    float mPursuitDuration;
    float mCooldownTimeRemaining;
    float mCooldownTimeRequired;
    int mNumCopsFullyEngaged;
    int mNumCopsDestroyed;
    int mNumCopsDamaged;
    int mTotalNumCopsInvolved;
    bool mHeliInvolved;
    int mPursuitRep;
    FEGroup * mpDataPursuitBoardGroup;
    FEGroup * mpDataPursuitMeterGroup;
    FEGroup * mpDataPursuitIconsGroup;
    FEGroup * mpDataPursuitSummaryGroup;
    FEGroup * mpDataPursuitCooldownMeterGroup;
    FEGroup * mpDataBackupTimerTextGroup;
    FEString * mpDataPursuitTimer;
    FEString * mpDataBackupTimer;
    FEString * mpDataPursuitCopsNumbers;
    FEString * mpDataCopsTakenOut;
    FEString * mpDataCopsDamaged;
    FEString * mpDataPursuitSummaryTotal;
    FEObject * mpDataBustedBar0;
    FEObject * mpDataBustedBar1;
    FEObject * mpDataBustedBar2;
    FEObject * mpDataBustedBar3;
    FEObject * mpDataBustedBar4;
    FEObject * mpDataCooldownBar;
    FEObject * mpDataBackupBacking;
    FEObject * mpDataHidingBacking;
    float mBustedBarOriginalWidth0;
    float mBustedBarOriginalWidth1;
    float mBustedBarOriginalWidth2;
    float mBustedBarOriginalWidth3;
    float mBustedBarOriginalWidth4;
    float mCooldownBarOriginalWidth;
};

#endif
