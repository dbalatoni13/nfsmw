#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

PursuitBoard::PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x100000), IPursuitBoard(pOutter) {
    mInPursuit = false;
    mIsHiding = false;
    mTimeUntilHidden = 0.0f;
    mTimeUntilBusted = 0.0f;
    mTimeUntilBackup = 0.0f;
    mPursuitDuration = 0.0f;
    mCooldownTimeRemaining = 0.0f;
    mCooldownTimeRequired = 60.0f;
    mNumCopsFullyEngaged = 0;
    mNumCopsDestroyed = 0;
    mNumCopsDamaged = 0;
    mTotalNumCopsInvolved = 0;
    mHeliInvolved = false;
    mPursuitRep = 0;
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

        if (!FEngIsScriptSet(mpDataPursuitBoardGroup, FEHASH_APPEAR)) {
            FEngSetScript(mpDataPursuitBoardGroup, FEHASH_APPEAR, true);
        }

        timer = Timer(mPursuitDuration);
        timer.PrintToString(timeToPrint, 4);
        FEPrintf(mpDataPursuitTimer, "%s", timeToPrint);
        FEPrintf(mpDataPursuitSummaryTotal, "%$d", mPursuitRep);

        if (!FEngIsScriptSet(mpDataPursuitSummaryGroup, FEHASH_APPEAR)) {
            FEngSetScript(mpDataPursuitSummaryGroup, FEHASH_APPEAR, true);
        }

        if (mTimeUntilBusted <= -1.0f) {
            float originalLeftX;

            if (!FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, FEHASH_ROTATE_IN)) {
                FEngSetScript(mpDataPursuitMeterGroup, FEHASH_ROTATE_OUT, true);
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0xc));
            }

            originalLeftX = FEngGetTopLeftX(mpDataCooldownBar);
            FEngSetSizeX(mpDataCooldownBar, mCooldownBarOriginalWidth * (1.0f - mCooldownTimeRemaining / mCooldownTimeRequired));
            FEngSetTopLeftX(mpDataCooldownBar, originalLeftX);

            if (mTimeUntilHidden > 0.0f) {
                if (!FEngIsScriptSet(mpDataHidingBacking, FEHASH_APPEAR)) {
                    FEngSetScript(mpDataHidingBacking, FEHASH_APPEAR, true);
                }
            } else {
                if (FEngIsScriptSet(mpDataHidingBacking, FEHASH_APPEAR)) {
                    FEngSetScript(mpDataHidingBacking, 0x33113ac, true);
                }
            }
        } else {
            int numCopsToReport;
            float originalLeftX;
            float originalRightX;
            float bustedBarTime;

            if (FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, FEHASH_ROTATE_IN)) {
                FEngSetScript(mpDataPursuitCooldownMeterGroup, FEHASH_ROTATE_OUT, true);
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0xc));
            } else {
                if (FEngIsScriptSet(mpDataPursuitMeterGroup, 0x16a259) || FEngIsScriptSet(mpDataPursuitMeterGroup, 0x33113ac)) {
                    FEngSetScript(mpDataPursuitMeterGroup, FEHASH_APPEAR, true);
                }
            }

            if (FEngIsScriptSet(mpDataHidingBacking, FEHASH_APPEAR)) {
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
                    if (!FEngIsScriptSet(mpDataBackupBacking, FEHASH_APPEAR)) {
                        FEngSetScript(mpDataBackupBacking, FEHASH_APPEAR, true);
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
        if (FEngIsScriptSet(mpDataPursuitBoardGroup, FEHASH_APPEAR)) {
            FEngSetScript(mpDataPursuitBoardGroup, 0x33113ac, true);
        }
        if (!FEngIsScriptSet(mpDataPursuitMeterGroup, 0x33113ac) && !FEngIsScriptSet(mpDataPursuitMeterGroup, 0x16a259)) {
            FEngSetScript(mpDataPursuitMeterGroup, 0x33113ac, true);
        }
        if (FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, FEHASH_APPEAR) || FEngIsScriptSet(mpDataPursuitCooldownMeterGroup, FEHASH_ROTATE_IN)) {
            FEngSetScript(mpDataPursuitCooldownMeterGroup, 0x33113ac, true);
        }
        if (FEngIsScriptSet(mpDataPursuitIconsGroup, FEHASH_APPEAR)) {
            FEngSetScript(mpDataPursuitIconsGroup, 0x33113ac, true);
        }
    }
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
            if (IPlayer::First(PLAYER_LOCAL)->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(GetTranslatedString(0x532b5186), false, FEHASH_FLASHER1, 0, 0, GenericMessage_Priority_1);
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

void PursuitBoard::SetPursuitDuration(float time) {
    if (mPursuitDuration != time) {
        if (time >= 0.0f) {
            mPursuitDuration = time;
        } else {
            mPursuitDuration = 0.0f;
        }
    }
}

void PursuitBoard::SetCooldownTimeRemaining(float time) {
    if (mCooldownTimeRemaining != time) {
        mCooldownTimeRemaining = time;
    }
}

void PursuitBoard::SetCooldownTimeRequired(float time) {
    mCooldownTimeRequired = time;
}

void PursuitBoard::SetNumCopsInPursuit(int numCops) {
    if (mNumCopsFullyEngaged != numCops) {
        if (numCops > mNumCopsFullyEngaged) {
            if (!FEngIsScriptRunning(GetPackageName(), 0x3787231c, FEHASH_ACTIVE)) {
                FEngSetScript(GetPackageName(), 0x3787231c, FEHASH_ACTIVE, true);
            }
        } else {
            if (!FEngIsScriptSet(GetPackageName(), 0x3787231c, FEHASH_INACTIVE)) {
                FEngSetScript(GetPackageName(), 0x3787231c, FEHASH_INACTIVE, true);
            }
            if (numCops < mNumCopsFullyEngaged) {
                if (!FEngIsScriptSet(GetPackageName(), 0x3b9919a8, FEHASH_COP_REMOVED)) {
                    FEngSetScript(GetPackageName(), 0x3b9919a8, FEHASH_COP_REMOVED, true);
                }
            }
        }
        mNumCopsFullyEngaged = numCops;
    }
}

void PursuitBoard::SetNumCopsDestroyed(int numCops, UCrc32 lastCopDestroyedType, int lastCopDestroyedMultiplier, int lastCopDestroyedRep) {
    if (mNumCopsDestroyed == numCops) {
        return;
    }
    if (numCops > mNumCopsDestroyed) {
        const char *pCopString = nullptr;
        if (lastCopDestroyedType == UCrc32("copcross")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPCROSS);
        } else if (lastCopDestroyedType == UCrc32("copsport")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPCROSS);
        } else if (lastCopDestroyedType == UCrc32("copmidsize")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPMIDSIZE);
        } else if (lastCopDestroyedType == UCrc32("copghost")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPGHOST);
        } else if (lastCopDestroyedType == UCrc32("copgto")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPGTO);
        } else if (lastCopDestroyedType == UCrc32("copgtoghost")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPGTOGHOST);
        } else if (lastCopDestroyedType == UCrc32("copsporthench")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPSPORTHENCH);
        } else if (lastCopDestroyedType == UCrc32("copsportghost")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPSPORTGHOST);
        } else if (lastCopDestroyedType == UCrc32("copsuv")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPSUV);
        } else if (lastCopDestroyedType == UCrc32("copsuvpatrol")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPSUV);
        } else if (lastCopDestroyedType == UCrc32("copsuvl")) {
            pCopString = GetLocalizedString(LANGUAGE_HUD_COPDESTROYED_COPSUVL);
        }
        if (pCopString) {
            char copCarString[64];
            bSNPrintf(copCarString, 64, pCopString, lastCopDestroyedRep * lastCopDestroyedMultiplier);
            IGenericMessage *igenericmessage;
            if (IPlayer::First(PLAYER_LOCAL)->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(copCarString, false, FEHASH_ZOOMIN, bStringHash("COPS_TAKENOUT_ICON"), 0x13ff94,
                                                       GenericMessage_Priority_4);
            }
        }
    }
    mNumCopsDestroyed = numCops;
}

void PursuitBoard::SetNumCopsDamaged(int numCops) {
    if (mNumCopsDamaged != numCops) {
        if (numCops > mNumCopsDamaged) {
            if (!FEngIsScriptSet(mpDataCopsDamaged, FEHASH_ACTIVE)) {
                FEngSetScript(mpDataCopsDamaged, FEHASH_ACTIVE, true);
            }
        }
        mNumCopsDamaged = numCops;
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
