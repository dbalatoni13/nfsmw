#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
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

inline float FEngGetTopLeftX(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return x;
}

inline float FEngGetTopLeftY(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return y;
}

inline float FEngGetBottomRightX(FEObject *obj) {
    float x, y;
    FEngGetBottomRight(obj, x, y);
    return x;
}

inline float FEngGetBottomRightY(FEObject *obj) {
    float x, y;
    FEngGetBottomRight(obj, x, y);
    return y;
}

inline float FEngGetSizeY(FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return y;
}

inline void FEngSetSizeX(FEObject *obj, float x) {
    float y = FEngGetSizeY(obj);
    FEngSetSize(obj, x, y);
}

inline void FEngSetTopLeftX(FEObject *obj, float x) {
    float y = FEngGetTopLeftY(obj);
    FEngSetTopLeft(obj, x, y);
}

inline void FEngSetBottomRightX(FEObject *obj, float x) {
    float y = FEngGetBottomRightY(obj);
    FEngSetBottomRight(obj, x, y);
}

PursuitBoard::PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x100000) //
    , IPursuitBoard(pOutter) //
    , mInPursuit(false) //
    , mIsHiding(false) //
    , mTimeUntilHidden(0.0f) //
    , mTimeUntilBusted(0.0f) //
    , mTimeUntilBackup(0.0f) //
    , mPursuitDuration(0.0f) //
    , mCooldownTimeRemaining(0.0f) //
    , mCooldownTimeRequired(60.0f) //
    , mNumCopsFullyEngaged(0) //
    , mNumCopsDestroyed(0) //
    , mNumCopsDamaged(0) //
    , mTotalNumCopsInvolved(0) //
    , mHeliInvolved(false) //
    , mPursuitRep(0)
{
    mpDataPursuitBoardGroup = RegisterGroup(0xde89e070);
    mpDataPursuitMeterGroup = RegisterGroup(0x7b422ba3);
    mpDataPursuitIconsGroup = RegisterGroup(0xe0b1430b);
    mpDataPursuitSummaryGroup = RegisterGroup(0x8674e6d4);
    mpDataPursuitCooldownMeterGroup = RegisterGroup(0x84a226ec);
    mpDataBackupTimerTextGroup = RegisterGroup(0x6a144066);
    RegisterString(0x3c165f39);
    RegisterString(0xbee44775);
    mpDataPursuitTimer = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0xfc39cb0a));
    mpDataBackupTimer = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0xbee44775));
    mpDataPursuitCopsNumbers = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0x814918ca));
    mpDataCopsTakenOut = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0xa16f9f1e));
    mpDataCopsDamaged = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0x5fa70d4c));
    mpDataPursuitSummaryTotal = static_cast<FEString *>(FEngFindObject(GetPackageName(), 0x875e92eb));
    mpDataBustedBar0 = FEngFindObject(GetPackageName(), 0xe0e0169b);
    mpDataBustedBar1 = FEngFindObject(GetPackageName(), 0x8eeebd33);
    mpDataBustedBar2 = FEngFindObject(GetPackageName(), 0x47a4e2a9);
    mpDataBustedBar3 = FEngFindObject(GetPackageName(), 0x8e3653a6);
    mpDataBustedBar4 = FEngFindObject(GetPackageName(), 0x30f39a6f);
    mpDataCooldownBar = FEngFindObject(GetPackageName(), 0xcf817638);
    mpDataBackupBacking = FEngFindObject(GetPackageName(), 0x8c20a763);
    mpDataHidingBacking = FEngFindObject(GetPackageName(), 0x5c2a4f20);
    mBustedBarOriginalWidth0 = mpDataBustedBar0->GetObjData()->Size.x;
    mBustedBarOriginalWidth1 = mpDataBustedBar1->GetObjData()->Size.x;
    mBustedBarOriginalWidth2 = mpDataBustedBar2->GetObjData()->Size.x;
    mBustedBarOriginalWidth3 = mpDataBustedBar3->GetObjData()->Size.x;
    mBustedBarOriginalWidth4 = mpDataBustedBar4->GetObjData()->Size.x;
    mCooldownBarOriginalWidth = mpDataCooldownBar->GetObjData()->Size.x;
}

void PursuitBoard::Update(IPlayer *player) {
    if (mInPursuit) {
        Timer timer;
        char timeToPrint[16];

        if (!FEngIsScriptSet(mpDataPursuitBoardGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitBoardGroup, 0x5079c8f8, true);
        }

        timer = Timer(mPursuitDuration);
        timer.PrintToString(timeToPrint, 4);
        FEPrintf(mpDataPursuitTimer, "%s", timeToPrint);
        FEPrintf(mpDataPursuitSummaryTotal, "%$d", mPursuitRep);

        if (!FEngIsScriptSet(mpDataPursuitSummaryGroup, 0x5079c8f8)) {
            FEngSetScript(mpDataPursuitSummaryGroup, 0x5079c8f8, true);
        }

        if (mTimeUntilBusted <= -1.0f) {
            float originalLeftX;

            if (!FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, 0x13f51124)) {
                FEngSetScript(mpDataPursuitMeterGroup, 0x92975065, true);
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0xc));
            }

            originalLeftX = FEngGetTopLeftX(mpDataCooldownBar);
            FEngSetSizeX(mpDataCooldownBar, mCooldownBarOriginalWidth * (1.0f - mCooldownTimeRemaining / mCooldownTimeRequired));
            FEngSetTopLeftX(mpDataCooldownBar, originalLeftX);

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
            int numCopsToReport;
            float originalLeftX;
            float originalRightX;
            float bustedBarTime;

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

            numCopsToReport = mNumCopsFullyEngaged;
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
                timer = Timer(mTimeUntilBackup);
                timer.PrintToString(timeToPrint, 4);
                FEPrintf(mpDataBackupTimer, "%s", timeToPrint);
            } else {
                if (!FEngIsScriptSet(mpDataBackupBacking, 0x33113ac)) {
                    FEngSetScript(mpDataBackupBacking, 0x33113ac, true);
                }
            }

            originalLeftX = 0.0f;
            originalRightX = FEngGetBottomRightX(mpDataBustedBar0);
            if (mTimeUntilBusted > 0.5f) {
                bustedBarTime = (mTimeUntilBusted - 0.5f) * 2.0f;
            } else {
                bustedBarTime = originalLeftX;
            }
            FEngSetSizeX(mpDataBustedBar0, bustedBarTime * mBustedBarOriginalWidth0);
            FEngSetBottomRightX(mpDataBustedBar0, originalRightX);

            if (mTimeUntilBusted > 0.5f) {
                if (!FEngIsScriptSet(mpDataBustedBar0, 0x26ded57)) {
                    FEngSetScript(mpDataBustedBar0, 0x26ded57, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataBustedBar0, 0x1744b3)) {
                    FEngSetScript(mpDataBustedBar0, 0x1744b3, true);
                }
            }

            originalRightX = FEngGetBottomRightX(mpDataBustedBar1);
            if (mTimeUntilBusted > 0.5f) {
                bustedBarTime = 0.9f;
            } else if (mTimeUntilBusted > 0.1f) {
                bustedBarTime = mTimeUntilBusted * 2.0f - 0.1f;
            } else {
                bustedBarTime = 0.0f;
            }
            FEngSetSizeX(mpDataBustedBar1, bustedBarTime * mBustedBarOriginalWidth1);
            FEngSetBottomRightX(mpDataBustedBar1, originalRightX);

            if (mTimeUntilBusted < 0.1f && mTimeUntilBusted > -0.1f) {
                if (!FEngIsScriptSet(mpDataBustedBar2, 0x3826a28)) {
                    FEngSetScript(mpDataBustedBar2, 0x3826a28, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataBustedBar2, 0x1ca7c0)) {
                    FEngSetScript(mpDataBustedBar2, 0x1ca7c0, true);
                }
            }

            originalLeftX = FEngGetTopLeftX(mpDataBustedBar3);
            if (mTimeUntilBusted < -0.5f) {
                bustedBarTime = 1.0f;
            } else if (mTimeUntilBusted < -0.1f) {
                bustedBarTime = (-mTimeUntilBusted * 2.0f) - 0.1f;
            } else {
                bustedBarTime = 0.0f;
            }
            FEngSetSizeX(mpDataBustedBar3, bustedBarTime * mBustedBarOriginalWidth3);
            FEngSetTopLeftX(mpDataBustedBar3, originalLeftX);

            originalLeftX = FEngGetTopLeftX(mpDataBustedBar4);
            if (mTimeUntilBusted < -0.5f) {
                bustedBarTime = -(mTimeUntilBusted + 0.5f) * 2.0f;
            } else {
                bustedBarTime = 0.0f;
            }
            FEngSetSizeX(mpDataBustedBar4, bustedBarTime * mBustedBarOriginalWidth4);
            FEngSetTopLeftX(mpDataBustedBar4, originalLeftX);

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

void PursuitBoard::SetTimeUntilBusted(float time, bool bIsBusted) {
    if (bIsBusted) {
        time = 1.0f;
    } else {
        if (time > 0.99f) {
            time = 0.99f;
        }
    }
    if (mTimeUntilBusted != time) {
        mTimeUntilBusted = time;
        if (time >= 1.0f) {
            IGenericMessage *igenericmessage;
            if (IPlayer::First(PLAYER_LOCAL)->GetSimable()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    GetTranslatedString(0x532b5186), false, 0x9d73bc15, 0, 0, GenericMessage_Priority_1);
            }
        }
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
            if (!FEngIsScriptRunning(GetPackageName(), 0x3787231c, 0x4f90cf9b)) {
                FEngSetScript(GetPackageName(), 0x3787231c, 0x4f90cf9b, true);
            }
        } else {
            if (!FEngIsScriptSet(GetPackageName(), 0x3787231c, 0xfb12d252)) {
                FEngSetScript(GetPackageName(), 0x3787231c, 0xfb12d252, true);
            }
            if (numCops < mNumCopsFullyEngaged) {
                if (!FEngIsScriptSet(GetPackageName(), 0x3b9919a8, 0x579dbc92)) {
                    FEngSetScript(GetPackageName(), 0x3b9919a8, 0x579dbc92, true);
                }
            }
        }
        mNumCopsFullyEngaged = numCops;
    }
}

void PursuitBoard::SetNumCopsDestroyed(int numCops, UCrc32 lastCopDestroyedType,
                                       int lastCopDestroyedMultiplier,
                                       int lastCopDestroyedRep) {
    if (mNumCopsDestroyed == numCops) {
        return;
    }
    if (numCops > mNumCopsDestroyed) {
        const char *pCopString = nullptr;
        if (lastCopDestroyedType == UCrc32("copcross")) {
            pCopString = GetLocalizedString(0x8fe02b9f);
        } else if (lastCopDestroyedType == UCrc32("copsport")) {
            pCopString = GetLocalizedString(0x8fe02b9f);
        } else if (lastCopDestroyedType == UCrc32("copmidsize")) {
            pCopString = GetLocalizedString(0xf49a550a);
        } else if (lastCopDestroyedType == UCrc32("copghost")) {
            pCopString = GetLocalizedString(0x902311da);
        } else if (lastCopDestroyedType == UCrc32("copgto")) {
            pCopString = GetLocalizedString(0x9bfd379f);
        } else if (lastCopDestroyedType == UCrc32("copgtoghost")) {
            pCopString = GetLocalizedString(0x65e97524);
        } else if (lastCopDestroyedType == UCrc32("copsporthench")) {
            pCopString = GetLocalizedString(0x4ee07213);
        } else if (lastCopDestroyedType == UCrc32("copsportghost")) {
            pCopString = GetLocalizedString(0x4ed00512);
        } else if (lastCopDestroyedType == UCrc32("copsuv")) {
            pCopString = GetLocalizedString(0x9bfd6ad3);
        } else if (lastCopDestroyedType == UCrc32("copsuvpatrol")) {
            pCopString = GetLocalizedString(0x9bfd6ad3);
        } else if (lastCopDestroyedType == UCrc32("copsuvl")) {
            pCopString = GetLocalizedString(0x1baac57f);
        }
        if (pCopString) {
            char copCarString[64];
            bSNPrintf(copCarString, 64, pCopString, lastCopDestroyedRep * lastCopDestroyedMultiplier);
            IGenericMessage *igenericmessage;
            if (IPlayer::First(PLAYER_LOCAL)->GetSimable()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    copCarString, false, 0x8ab83edb,
                    bStringHash("COPS_TAKENOUT_ICON"), 0x13ff94,
                    GenericMessage_Priority_4);
            }
        }
    }
    mNumCopsDestroyed = numCops;
}
