#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"

void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
bool FEngIsScriptRunning(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
bool FEngIsScriptRunning(FEObject *object, unsigned int script_hash);

PursuitBoard::PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IPursuitBoard(pOutter)
{
}

void PursuitBoard::Update(IPlayer *player) {
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
