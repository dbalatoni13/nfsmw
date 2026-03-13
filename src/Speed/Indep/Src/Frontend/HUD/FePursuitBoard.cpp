#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
bool FEngIsScriptRunning(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
bool FEngIsScriptRunning(FEObject *object, unsigned int script_hash);
int FEPrintf(FEString *text, const char *fmt, ...);
void FEngGetBottomRight(FEObject *object, float &x, float &y);
void FEngSetBottomRight(FEObject *object, float x, float y);
void FEngGetTopLeft(FEObject *object, float &x, float &y);
void FEngSetTopLeft(FEObject *object, float x, float y);
void FEngGetSize(FEObject *object, float &x, float &y);
void FEngSetSize(FEObject *object, float x, float y);

PursuitBoard::PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IPursuitBoard(pOutter)
{
}

void PursuitBoard::Update(IPlayer *player) {
    if (mInPursuit) {
        Timer timer;
        char timeToPrint[16];
        float x, y;
        float originalRightX;
        float originalLeftX;
        float bustedBarTime;

        if (!FEngIsScriptSet(mpDataPursuitBoardGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitBoardGroup, 0x5079c8f8, true);
        }

        timer.SetTime(mPursuitDuration);
        timer.PrintToString(timeToPrint, 4);
        FEPrintf(mpDataPursuitTimer, "%s", timeToPrint);
        FEPrintf(mpDataPursuitSummaryTotal, "%$d", mPursuitRep);

        if (!FEngIsScriptSet(mpDataPursuitSummaryGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitSummaryGroup, 0x5079c8f8, true);
        }

        if (mTimeUntilBusted <= -1.0f) {
            if (!FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, 0x13f51124)) {
                FEngSetScript(mpDataPursuitMeterGroup, 0x92975065, true);
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0xc));
            }

            FEngGetTopLeft(mpDataCooldownBar, x, y);
            originalLeftX = x;
            float cooldownAmount = 1.0f - mCooldownTimeRemaining / mCooldownTimeRequired;
            FEngGetSize(mpDataCooldownBar, x, y);
            FEngSetSize(mpDataCooldownBar, mCooldownBarOriginalWidth * cooldownAmount, y);
            FEngGetTopLeft(mpDataCooldownBar, x, y);
            FEngSetTopLeft(mpDataCooldownBar, originalLeftX, y);

            if (mTimeUntilHidden > 0.0f) {
                if (!FEngIsScriptSet(mpDataHidingBacking, 0x5079c8f8)) {
                    FEngSetScript(mpDataHidingBacking, 0x5079c8f8, true);
                }
            } else {
                if (FEngIsScriptSet(mpDataHidingBacking, 0x5079c8f8)) {
                    FEngSetScript(mpDataHidingBacking, 0x33113ac, true);
                }
            }
        } else {
            if (FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, 0x13f51124)) {
                FEngSetScript(mpDataPursuitCooldownMeterGroup, 0x92975065, true);
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0xc));
            } else {
                if (FEngIsScriptSet(mpDataPursuitMeterGroup, 0x16a259) ||
                    FEngIsScriptSet(mpDataPursuitMeterGroup, 0x33113ac)) {
                    FEngSetScript(mpDataPursuitMeterGroup, 0x5079c8f8, true);
                }
            }

            if (FEngIsScriptSet(mpDataHidingBacking, 0x5079c8f8)) {
                FEngSetScript(mpDataHidingBacking, 0x33113ac, true);
            }

            int numCopsToReport = mNumCopsFullyEngaged;
            if (mHeliInvolved) {
                numCopsToReport = numCopsToReport - 1;
            }
            FEPrintf(mpDataPursuitCopsNumbers, "%$d", numCopsToReport);
            FEPrintf(mpDataCopsTakenOut, "%$d", mNumCopsDestroyed);
            FEPrintf(mpDataCopsDamaged, "%$d", mNumCopsDamaged);

            if (mTimeUntilBackup > 0.0f) {
                if (mTimeUntilBackup > 10.0f) {
                    if (!FEngIsScriptSet(mpDataBackupBacking, 0x5079c8f8)) {
                        FEngSetScript(mpDataBackupBacking, 0x5079c8f8, true);
                    }
                } else {
                    if (!FEngIsScriptSet(mpDataBackupBacking, 0x26ded57)) {
                        FEngSetScript(mpDataBackupBacking, 0x26ded57, true);
                    }
                }
                timer.SetTime(mTimeUntilBackup);
                timer.PrintToString(timeToPrint, 4);
                FEPrintf(mpDataBackupTimer, "%s", timeToPrint);
            } else {
                if (!FEngIsScriptSet(mpDataBackupBacking, 0x33113ac)) {
                    FEngSetScript(mpDataBackupBacking, 0x33113ac, true);
                }
            }

            FEngGetBottomRight(mpDataBustedBar0, x, y);
            originalRightX = x;
            if (mTimeUntilBusted > 0.5f) {
                bustedBarTime = (mTimeUntilBusted - 0.5f) * 2.0f;
            } else {
                bustedBarTime = 0.0f;
            }
            FEngGetSize(mpDataBustedBar0, x, y);
            FEngSetSize(mpDataBustedBar0, bustedBarTime * mBustedBarOriginalWidth0, y);
            FEngGetBottomRight(mpDataBustedBar0, x, y);
            FEngSetBottomRight(mpDataBustedBar0, originalRightX, y);

            if (mTimeUntilBusted > 0.5f) {
                if (!FEngIsScriptSet(mpDataBustedBar0, 0x26ded57)) {
                    FEngSetScript(mpDataBustedBar0, 0x26ded57, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataBustedBar0, 0x1744b3)) {
                    FEngSetScript(mpDataBustedBar0, 0x1744b3, true);
                }
            }

            FEngGetBottomRight(mpDataBustedBar1, x, y);
            originalRightX = x;
            if (mTimeUntilBusted > 0.5f) {
                bustedBarTime = 0.9f;
            } else if (mTimeUntilBusted > 0.1f) {
                bustedBarTime = mTimeUntilBusted * 2.0f - 0.1f;
            } else {
                bustedBarTime = 0.0f;
            }
            FEngGetSize(mpDataBustedBar1, x, y);
            FEngSetSize(mpDataBustedBar1, bustedBarTime * mBustedBarOriginalWidth1, y);
            FEngGetBottomRight(mpDataBustedBar1, x, y);
            FEngSetBottomRight(mpDataBustedBar1, originalRightX, y);

            if (mTimeUntilBusted >= 0.1f || mTimeUntilBusted <= -0.1f) {
                if (!FEngIsScriptSet(mpDataBustedBar2, 0x1ca7c0)) {
                    FEngSetScript(mpDataBustedBar2, 0x1ca7c0, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataBustedBar2, 0x3826a28)) {
                    FEngSetScript(mpDataBustedBar2, 0x3826a28, true);
                }
            }

            FEngGetTopLeft(mpDataBustedBar3, x, y);
            originalLeftX = x;
            bustedBarTime = 1.0f;
            if (mTimeUntilBusted >= -0.5f) {
                if (mTimeUntilBusted < -0.1f) {
                    bustedBarTime = (-mTimeUntilBusted * 2.0f) - 0.1f;
                } else {
                    bustedBarTime = 0.0f;
                }
            }
            FEngGetSize(mpDataBustedBar3, x, y);
            FEngSetSize(mpDataBustedBar3, bustedBarTime * mBustedBarOriginalWidth3, y);
            FEngGetTopLeft(mpDataBustedBar3, x, y);
            FEngSetTopLeft(mpDataBustedBar3, originalLeftX, y);

            FEngGetTopLeft(mpDataBustedBar4, x, y);
            originalLeftX = x;
            bustedBarTime = 0.0f;
            if (mTimeUntilBusted < -0.5f) {
                bustedBarTime = -(mTimeUntilBusted + 0.5f) * 2.0f;
            }
            FEngGetSize(mpDataBustedBar4, x, y);
            FEngSetSize(mpDataBustedBar4, bustedBarTime * mBustedBarOriginalWidth4, y);
            FEngGetTopLeft(mpDataBustedBar4, x, y);
            FEngSetTopLeft(mpDataBustedBar4, originalLeftX, y);

            if (mTimeUntilBusted < -0.5f) {
                if (!FEngIsScriptSet(mpDataBustedBar4, 0x26ded57)) {
                    FEngSetScript(mpDataBustedBar4, 0x26ded57, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataBustedBar4, 0x1744b3)) {
                    FEngSetScript(mpDataBustedBar4, 0x1744b3, true);
                }
            }
        }
    } else {
        if (FEngIsScriptSet(mpDataPursuitBoardGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitBoardGroup, 0x33113ac, true);
        }
        if (!FEngIsScriptSet(mpDataPursuitMeterGroup, 0x33113ac) &&
            !FEngIsScriptSet(mpDataPursuitMeterGroup, 0x16a259)) {
            FEngSetScript(mpDataPursuitMeterGroup, 0x33113ac, true);
        }
        if (FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, 0x5079c8f8) ||
            FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, 0x13f51124)) {
            FEngSetScript(mpDataPursuitCooldownMeterGroup, 0x33113ac, true);
        }
        if (FEngIsScriptSet(mpDataPursuitIconsGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitIconsGroup, 0x33113ac, true);
        }
    }
}

void PursuitBoard::SetCooldownTimeRequired(float time) {
    mCooldownTimeRequired = time;
}

void PursuitBoard::SetInPursuit(bool inPursuit) {
    if (mInPursuit != inPursuit) {
        mInPursuit = inPursuit;
    }
}

void PursuitBoard::SetIsHiding(bool isHiding) {
    if (mIsHiding != isHiding) {
        mIsHiding = isHiding;
    }
}

void PursuitBoard::SetTimeUntilHidden(float time) {
    if (mTimeUntilHidden != time) {
        mTimeUntilHidden = time;
    }
}

void PursuitBoard::SetTimeUntilBackup(float time) {
    if (mTimeUntilBackup != time) {
        mTimeUntilBackup = time;
    }
}

void PursuitBoard::SetIsInView(bool isInView) {
    if (mIsInView != isInView) {
        mIsInView = isInView;
    }
}

void PursuitBoard::SetCooldownTimeRemaining(float time) {
    if (mCooldownTimeRemaining != time) {
        mCooldownTimeRemaining = time;
    }
}

void PursuitBoard::SetTotalNumCopsInvolved(int numCops) {
    if (mTotalNumCopsInvolved != numCops) {
        mTotalNumCopsInvolved = numCops;
    }
}

void PursuitBoard::SetHeliInvolvedInPursuit(bool heliInvolved) {
    if (mHeliInvolved != heliInvolved) {
        mHeliInvolved = heliInvolved;
    }
}

void PursuitBoard::SetPursuitRep(int rep) {
    if (mPursuitRep != rep) {
        mPursuitRep = rep;
    }
}

void PursuitBoard::SetPursuitDuration(float time) {
    if (mPursuitDuration != time) {
        if (time >= 0.0f) {
            mPursuitDuration = time;
        } else {
            mPursuitDuration = 0.0f;
        }
    }
}

void PursuitBoard::SetNumCopsDamaged(int numCops) {
    if (mNumCopsDamaged != numCops) {
        if (numCops > mNumCopsDamaged) {
            if (!FEngIsScriptSet(mpDataCopsDamaged, 0x4f90cf9b)) {
                FEngSetScript(mpDataCopsDamaged, 0x4f90cf9b, true);
            }
        }
        mNumCopsDamaged = numCops;
    }
}

void PursuitBoard::SetNumCopsInPursuit(int numCops) {
    if (mNumCopsFullyEngaged != numCops) {
        if (numCops > mNumCopsFullyEngaged) {
            if (!FEngIsScriptRunning(pPackageName, 0x3787231c, 0x4f90cf9b)) {
                FEngSetScript(pPackageName, 0x3787231c, 0x4f90cf9b, true);
            }
        } else {
            if (!FEngIsScriptSet(pPackageName, 0x3787231c, 0xfb12d252)) {
                FEngSetScript(pPackageName, 0x3787231c, 0xfb12d252, true);
            }
            if (mNumCopsFullyEngaged > numCops) {
                if (!FEngIsScriptSet(pPackageName, 0x3b9919a8, 0x579dbc92)) {
                    FEngSetScript(pPackageName, 0x3b9919a8, 0x579dbc92, true);
                }
            }
        }
        mNumCopsFullyEngaged = numCops;
    }
}
