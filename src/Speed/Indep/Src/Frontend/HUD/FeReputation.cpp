#include "Speed/Indep/Src/Frontend/HUD/FeReputation.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

Reputation::Reputation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number) : HudElement(pkg_name, 0x1000), IReputation(pOutter) {
    mReputationCareer = 0;
    mNumFramesLeftToShow = 0;
    mDataReputationGrp = RegisterGroup(0xEA903012);
    FEngSetScript(mDataReputationGrp, FEHASH_HIDE, true);
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
        FEPrintf(mDataReputationCareer, "%$d", mReputationCareer);
        if (!FEngIsScriptSet(mDataReputationGrp, FEHASH_APPEAR)) {
            FEngSetScript(mDataReputationGrp, FEHASH_APPEAR, true);
        }
    } else {
        if (FEngIsScriptSet(mDataReputationGrp, FEHASH_APPEAR)) {
            FEngSetScript(mDataReputationGrp, FEHASH_LEAVE, true);
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
