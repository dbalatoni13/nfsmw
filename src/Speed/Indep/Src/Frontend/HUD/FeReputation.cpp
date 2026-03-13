#include "Speed/Indep/Src/Frontend/HUD/FeReputation.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"

void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
int FEPrintf(FEString *text, const char *fmt, ...);
FEString *FEngFindString(const char *pkg_name, int name_hash);

extern const char lbl_803E48C8[];

Reputation::Reputation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x1000) //
    , IReputation(pOutter) //
    , mReputationCareer(0) //
    , mNumFramesLeftToShow(0) //
{
    mDataReputationGrp = RegisterGroup(0xEA903012);
    FEngSetScript(mDataReputationGrp, 0x16A259, true);
    mDataReputationCareer = FEngFindString(GetPackageName(), 0x9B0AC8CA);
    mDataTitle = FEngFindString(GetPackageName(), 0x41A55ECF);
}

void Reputation::Update(IPlayer *player) {
    if (mDataReputationCareer == nullptr) {
        return;
    }

    if (mNumFramesLeftToShow >= 1) {
        mNumFramesLeftToShow = mNumFramesLeftToShow - 1;
        FEngSetLanguageHash(mDataTitle, 0x7D0171E4);
        FEPrintf(mDataReputationCareer, lbl_803E48C8, mReputationCareer);
        if (!FEngIsScriptSet(mDataReputationGrp, 0x5079C8F8)) {
            FEngSetScript(mDataReputationGrp, 0x5079C8F8, true);
        }
    } else {
        if (FEngIsScriptSet(mDataReputationGrp, 0x5079C8F8)) {
            FEngSetScript(mDataReputationGrp, 0x33113AC, true);
        }
    }
}

void Reputation::SetReputationCareer(int rep) {
    if (mReputationCareer == rep) {
        return;
    }
    mReputationCareer = rep;
    mNumFramesLeftToShow = 0x78;
}

void Reputation::SetReputationPursuit(int rep) {}
