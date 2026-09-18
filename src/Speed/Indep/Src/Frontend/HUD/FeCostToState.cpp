#include "Speed/Indep/Src/Frontend/HUD/FeCostToState.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);


CostToState::CostToState(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x1000), //
      ICostToState(pOutter),        //
      mCostToStateOn(false),        //
      mCostToState(0),              //
      mInPursuit(false),            //
      mNumFramesLeftToShow(0)       //
{
    RegisterGroup(FEHashUpper("CTS_GROUP"));
    FEngSetScript(GetPackageName(), FEHashUpper("CTS_GROUP"), FEHashUpper("HIDE"), true);
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
        FEPrintf(mDataCostToState, "%$d", mCostToState);
        if (!mCostToStateOn) {
            mCostToStateOn = true;
            FEngSetScript(GetPackageName(), FEHashUpper("CTS_GROUP"), FEHashUpper("APPEAR"), true);
        }
    } else {
        if (mCostToStateOn) {
            mCostToStateOn = false;
            FEngSetScript(GetPackageName(), FEHashUpper("CTS_GROUP"), FEHashUpper("LEAVE"), true);
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
