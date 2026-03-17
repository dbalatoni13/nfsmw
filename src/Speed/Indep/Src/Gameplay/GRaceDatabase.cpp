#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

GRaceDatabase *GRaceDatabase::mObj = nullptr;

unsigned int GRaceBin::GetCollectionKey() const {
    return mBinRecord.GetCollection();
}

const Attrib::Gen::gameplay *GRaceBin::GetGameplayObj() const {
    return &mBinRecord;
}

int GRaceBin::GetBinNumber() const {
    return *reinterpret_cast<const int *>(mBinRecord.GetAttributePointer(0x6CE23062, 0) ?
                                              mBinRecord.GetAttributePointer(0x6CE23062, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

int GRaceBin::GetBossReputation() const {
    return mBinRecord.BossReputation(0);
}

float GRaceBin::GetBaseOpenWorldHeat() const {
    return *reinterpret_cast<const float *>(mBinRecord.GetAttributePointer(0x8F186AC4, 0) ?
                                                mBinRecord.GetAttributePointer(0x8F186AC4, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

float GRaceBin::GetMaxOpenWorldHeat() const {
    return *reinterpret_cast<const float *>(mBinRecord.GetAttributePointer(0xE8C24416, 0) ?
                                                mBinRecord.GetAttributePointer(0xE8C24416, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

float GRaceBin::GetScaleOpenWorldHeat() const {
    return *reinterpret_cast<const float *>(mBinRecord.GetAttributePointer(0x1823B89E, 0) ?
                                                mBinRecord.GetAttributePointer(0x1823B89E, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

unsigned int GRaceBin::GetBossRaceCount() const {
    Attrib::Attribute bossRaces;

    bossRaces = mBinRecord.Get(0xD5A174AA);
    return bossRaces.GetLength();
}

unsigned int GRaceBin::GetBossRaceHash(unsigned int index) const {
    GRaceParameters *race;
    const unsigned int *key;

    key = reinterpret_cast<const unsigned int *>(mBinRecord.GetAttributePointer(0xD5A174AA, index));
    if (!key) {
        key = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
    }

    race = GRaceDatabase::Get().GetRaceFromKey(*key);
    return race ? race->GetEventHash() : 0;
}

unsigned int GRaceBin::GetWorldRaceCount() const {
    Attrib::Attribute worldRaces;

    worldRaces = mBinRecord.Get(0xA7EF40EF);
    return worldRaces.GetLength();
}

unsigned int GRaceBin::GetWorldRaceHash(unsigned int index) const {
    GRaceParameters *race;
    const unsigned int *key;

    key = reinterpret_cast<const unsigned int *>(mBinRecord.GetAttributePointer(0xA7EF40EF, index));
    if (!key) {
        key = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
    }

    race = GRaceDatabase::Get().GetRaceFromKey(*key);
    return race ? race->GetEventHash() : 0;
}

unsigned int GRaceBin::GetBaselineUnlockCount() const {
    Attrib::Attribute unlocks;

    unlocks = mBinRecord.Get(0xBAF89280);
    return unlocks.GetLength();
}

unsigned int GRaceBin::GetBaselineUnlock(unsigned int index) const {
    const unsigned int *key;

    key = reinterpret_cast<const unsigned int *>(mBinRecord.GetAttributePointer(0xBAF89280, index));
    if (!key) {
        key = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
    }

    return *key;
}

unsigned int GRaceBin::GetBarrierCount() const {
    Attrib::Attribute barriers;

    barriers = mBinRecord.Get(0xE244F26B);
    return barriers.GetLength();
}

const char *GRaceBin::GetBarrierName(unsigned int index) const {
    const char *barrierName;
    const EA::Reflection::Text *barrierText;

    barrierText = reinterpret_cast<const EA::Reflection::Text *>(mBinRecord.GetAttributePointer(0xE244F26B, index));
    if (!barrierText) {
        barrierText = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    barrierName = *barrierText;
    if (barrierName && *barrierName == '*') {
        return barrierName + 1;
    }

    return barrierName;
}

unsigned int GRaceBin::GetBarrierHash(unsigned int index) const {
    const char *barrierName;

    barrierName = GetBarrierName(index);
    return barrierName ? Attrib::StringToKey(barrierName) : 0;
}

bool GRaceBin::GetBarrierIsFlipped(unsigned int index) const {
    const char *barrierName;
    const EA::Reflection::Text *barrierText;

    barrierText = reinterpret_cast<const EA::Reflection::Text *>(mBinRecord.GetAttributePointer(0xE244F26B, index));
    if (!barrierText) {
        barrierText = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    barrierName = *barrierText;
    return barrierName && *barrierName == '*';
}

int GRaceBin::GetRequiredBounty() const {
    return *reinterpret_cast<const int *>(mBinRecord.GetAttributePointer(0xD3657D92, 0) ?
                                              mBinRecord.GetAttributePointer(0xD3657D92, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

int GRaceBin::GetRequiredChallenges() const {
    return *reinterpret_cast<const int *>(mBinRecord.GetAttributePointer(0x6DD4B98B, 0) ?
                                              mBinRecord.GetAttributePointer(0x6DD4B98B, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

int GRaceBin::GetRequiredRaceWins() const {
    return *reinterpret_cast<const int *>(mBinRecord.GetAttributePointer(0xD617FEDC, 0) ?
                                              mBinRecord.GetAttributePointer(0xD617FEDC, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

GRaceCustom *GRaceDatabase::GetStartupRace() {
    return mStartupRace;
}

Context GRaceDatabase::GetStartupRaceContext() {
    return mStartupRaceContext;
}

unsigned int GRaceDatabase::GetBinCount() {
    return mBinCount;
}

unsigned int GRaceDatabase::GetRaceCount() {
    return mRaceCountStatic + mRaceCountDynamic;
}

GRaceBin *GRaceDatabase::GetBin(unsigned int index) {
    return &mBins[index];
}

GRaceParameters *GRaceDatabase::GetRaceParameters(unsigned int index) {
    if (mRaceCountStatic <= index) {
        return mRaceCustom[index - mRaceCountStatic];
    }

    return &mRaceParameters[index];
}

GRaceParameters *GRaceDatabase::GetRaceFromKey(unsigned int key) {
    unsigned int i;

    for (i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *race;

        race = GetRaceParameters(i);
        if (race && race->GetCollectionKey() == key) {
            return race;
        }
    }

    return nullptr;
}

void GRaceDatabase::DestroyCustomRace(GRaceCustom *custom) {
    unsigned int i;

    if (custom) {
        delete custom;
    }

    for (i = 0; i < 4; i++) {
        if (mRaceCustom[i] == custom) {
            unsigned int lastIndex;

            lastIndex = mRaceCountDynamic - 1;
            if (i < lastIndex) {
                mRaceCustom[i] = mRaceCustom[lastIndex];
            }
            mRaceCountDynamic = lastIndex;
            mRaceCustom[lastIndex] = nullptr;
            return;
        }
    }
}

void GRaceDatabase::ClearStartupRace() {
    if (mStartupRace && !mStartupRace->GetFreedByOwner()) {
        DestroyCustomRace(mStartupRace);
    }

    mStartupRaceContext = kRaceContext_QuickRace;
    mStartupRace = nullptr;
}

GRaceBin *GRaceDatabase::GetBinNumber(int number) {
    unsigned int i;

    for (i = 0; i < GetBinCount(); i++) {
        GRaceBin *bin;

        bin = &mBins[i];
        if (bin->GetBinNumber() == number) {
            return bin;
        }
    }

    return nullptr;
}

void GRaceDatabase::NotifyVaultLoaded(GVault *vault) {
    unsigned int i;

    for (i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *race;

        race = GetRaceParameters(i);
        if (race->GetParentVault() == vault) {
            race->NotifyParentVaultLoaded();
        }
    }
}

void GRaceDatabase::NotifyVaultUnloading(GVault *vault) {
    unsigned int i;

    for (i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *race;

        race = GetRaceParameters(i);
        if (race->GetParentVault() == vault) {
            race->NotifyParentVaultUnloading();
        }
    }

    if (mStartupRace && mStartupRace->GetParentVault() == vault) {
        ClearStartupRace();
    }
}

void GRaceDatabase::FreeCustomRace(GRaceCustom *custom) {
    if (!custom) {
        return;
    }

    if (custom == mStartupRace) {
        custom->SetFreedByOwner();
    } else {
        DestroyCustomRace(custom);
    }
}
