#include "Speed/Indep/Src/Frontend/HUD/FeCostToState.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"

unsigned long FEHashUpper(const char *name);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
int FEPrintf(FEString *text, const char *fmt, ...);
FEString *FEngFindString(const char *pkg_name, int name_hash);

extern const char lbl_803E48B4[];
extern const char lbl_803E48C0[];
extern const char lbl_803E48C8[];
extern const char lbl_803E48CC[];
extern const char lbl_803E48D4[];

CostToState::CostToState(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x1000) //
    , ICostToState(pOutter) //
    , mCostToStateOn(false) //
    , mCostToState(0) //
    , mInPursuit(false) //
    , mNumFramesLeftToShow(0) //
{
    RegisterGroup(FEHashUpper(lbl_803E48B4));
    FEngSetScript(GetPackageName(), FEHashUpper(lbl_803E48B4), FEHashUpper(lbl_803E48C0), true);
    mDataCostToState = FEngFindString(GetPackageName(), 0x3FF5F33C);
    mDataTitle = FEngFindString(GetPackageName(), 0x64247241);
}

void CostToState::Update(IPlayer *player) {
    if (mDataCostToState == nullptr) {
        return;
    }

    if (mNumFramesLeftToShow >= 1) {
        mNumFramesLeftToShow = mNumFramesLeftToShow - 1;
        FEngSetLanguageHash(mDataTitle, 0x3DD874C5);
        FEPrintf(mDataCostToState, lbl_803E48C8, mCostToState);
        if (!mCostToStateOn) {
            mCostToStateOn = true;
            FEngSetScript(GetPackageName(), FEHashUpper(lbl_803E48B4), FEHashUpper(lbl_803E48CC), true);
        }
    } else {
        if (mCostToStateOn) {
            mCostToStateOn = false;
            FEngSetScript(GetPackageName(), FEHashUpper(lbl_803E48B4), FEHashUpper(lbl_803E48D4), true);
        }
    }
}

void CostToState::SetCostToState(int cost) {
    if (!mInPursuit) {
        return;
    }
    if (cost > mCostToState) {
        mCostToState = cost;
        mNumFramesLeftToShow = 0x78;
        return;
    }
    if (cost != 0) {
        return;
    }
    mCostToState = 0;
}

void CostToState::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}
