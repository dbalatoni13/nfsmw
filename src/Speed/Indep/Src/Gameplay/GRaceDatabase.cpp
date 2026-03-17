#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <new>

void EnableBarrierSceneryGroup(const char *name, bool flipped);
void RedoTopologyAndSceneryGroups();

GRaceDatabase *GRaceDatabase::mObj = nullptr;
static const char *kDDayRaces[5] = {
    "16.1.0",
    "16.2.2",
    "16.2.3",
    "16.1.1",
    "16.2.1",
};

GRaceDatabase::GRaceDatabase()
    : mRaceCountStatic(0), //
      mRaceCountDynamic(0), //
      mRaceIndex(nullptr), //
      mRaceParameters(nullptr), //
      mBinCount(0), //
      mBins(nullptr), //
      mGameplayClass(Attrib::Database::Get().GetClass(0x5CEA9D46)), //
      mStartupRace(nullptr), //
      mStartupRaceContext(GRace::kRaceContext_QuickRace), //
      mNumInitialUnlocks(0), //
      mInitialUnlockHash(nullptr), //
      mRaceScoreInfo(nullptr) {
    unsigned int i;

    for (i = 0; i < 4; i++) {
        mRaceCustom[i] = nullptr;
    }

    BuildBinList();
    BuildRaceList();
    BuildScoreList();
}

GRaceBin::GRaceBin(unsigned int collectionKey)
    : mBinRecord(collectionKey, 0, nullptr), //
      mChildVault(nullptr) {
    bMemSet(&mStats, 0, sizeof(mStats));
}

unsigned int GRaceBin::GetCollectionKey() const {
    return mBinRecord.GetCollection();
}

const Attrib::Gen::gameplay *GRaceBin::GetGameplayObj() const {
    return &mBinRecord;
}

GVault *GRaceBin::GetChildVault() const {
    return mChildVault;
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

void GRaceBin::EnableBarriers() {
    unsigned int i;

    for (i = 0; i < GetBarrierCount(); i++) {
        EnableBarrierSceneryGroup(GetBarrierName(i), GetBarrierIsFlipped(i));
    }

    WCollisionAssets::Get().SetExclusionFlags();
    WRoadNetwork::Get().ResolveBarriers();
}

void GRaceBin::DisableBarriers() {
    RedoTopologyAndSceneryGroups();
    WRoadNetwork::Get().ResetBarriers();
    WRoadNetwork::Get().ResetRaceSegments();
}

unsigned int GRaceBin::Serialize(unsigned char *dest) {
    *reinterpret_cast<BinStats *>(dest) = mStats;
    return sizeof(mStats);
}

unsigned int GRaceBin::Deserialize(unsigned char *src) {
    mStats = *reinterpret_cast<BinStats *>(src);
    return sizeof(mStats);
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

int GRaceBin::GetCompletedChallenges() const {
    return mStats.mChallengesCompleted;
}

int GRaceBin::GetAwardedRaceWins() const {
    return mStats.mRacesWon;
}

void GRaceBin::SetCompletedChallenges(int numChallenges) {
    mStats.mChallengesCompleted = numChallenges;
}

void GRaceBin::SetRacesWon(int numRaces) {
    mStats.mRacesWon = numRaces;
}

void GRaceBin::RefreshProgress() {
    unsigned int racesWon;
    int completedChallenges;
    unsigned int binNumber;
    GMilestone *milestone;
    GSpeedTrap *speedTrap;

    racesWon = 0;
    for (binNumber = 0; binNumber < GetWorldRaceCount(); binNumber++) {
        if (GRaceDatabase::Get().CheckRaceScoreFlags(GetWorldRaceHash(binNumber), GRaceDatabase::kCompleted_ContextCareer)) {
            racesWon++;
        }
    }

    completedChallenges = 0;
    binNumber = GetBinNumber();
    milestone = GManager::Get().GetFirstMilestone(false, binNumber);
    while (milestone) {
        if (milestone->GetIsAwarded()) {
            completedChallenges++;
        }
        milestone = GManager::Get().GetNextMilestone(milestone, false, binNumber);
    }

    speedTrap = GManager::Get().GetFirstSpeedTrap(false, binNumber);
    while (speedTrap) {
        if (speedTrap->IsFlagSet(4)) {
            completedChallenges++;
        }
        speedTrap = GManager::Get().GetNextSpeedTrap(speedTrap, false, binNumber);
    }

    SetRacesWon(racesWon);
    SetCompletedChallenges(completedChallenges);
}

void GRaceDatabase::Init() {
    mObj = new GRaceDatabase;
}

GRaceCustom *GRaceDatabase::GetStartupRace() {
    return mStartupRace;
}

GRace::Context GRaceDatabase::GetStartupRaceContext() {
    return mStartupRaceContext;
}

unsigned int GRaceDatabase::GetBinCount() {
    return mBinCount;
}

unsigned int GRaceDatabase::GetRaceCount() {
    return mRaceCountStatic + mRaceCountDynamic;
}

void GRaceDatabase::SimulateDDayComplete() {}

GRaceBin *GRaceDatabase::GetBin(unsigned int index) {
    return &mBins[index];
}

bool GRaceDatabase::CollectionIsRaceActivity(Attrib::Gen::gameplay &collection) {
    return collection.GetAttributePointer(0xA78403EC, 0) != nullptr;
}

bool GRaceDatabase::CollectionIsRaceBin(Attrib::Gen::gameplay &collection) {
    return collection.GetAttributePointer(0x6CE23062, 0) != nullptr;
}

unsigned int GRaceDatabase::StoreBinList(GRaceBin *dest) {
    unsigned int count;
    unsigned int collectionKey;
    GRaceBin *current;

    count = 0;
    current = dest;
    for (collectionKey = mGameplayClass->GetFirstCollection(); collectionKey != 0;
         collectionKey = mGameplayClass->GetNextCollection(collectionKey)) {
        Attrib::Gen::gameplay gameplay(collectionKey, 0, nullptr);

        if (CollectionIsRaceBin(gameplay)) {
            if (dest && current) {
                new (current) GRaceBin(collectionKey);
                current = reinterpret_cast<GRaceBin *>(reinterpret_cast<unsigned char *>(current) + 0x1C);
            }
            count++;
        }
    }

    return count;
}

void GRaceDatabase::BuildBinList() {
    mBinCount = StoreBinList(nullptr);
    mBins = static_cast<GRaceBin *>(bMalloc(mBinCount * 0x1C, 0));
    StoreBinList(mBins);
}

unsigned int GRaceDatabase::StoreRaceList(GRaceParameters *dest) {
    unsigned int count;
    unsigned int collectionKey;
    GRaceParameters *current;
    unsigned char *indexData;

    count = 0;
    current = dest;
    indexData = reinterpret_cast<unsigned char *>(mRaceIndex);
    for (collectionKey = mGameplayClass->GetFirstCollection(); collectionKey != 0;
         collectionKey = mGameplayClass->GetNextCollection(collectionKey)) {
        Attrib::Gen::gameplay gameplay(collectionKey, 0, nullptr);

        if (CollectionIsRaceActivity(gameplay)) {
            if (dest && current) {
                new (current) GRaceParameters(collectionKey, reinterpret_cast<GRaceIndexData *>(indexData));
                current = reinterpret_cast<GRaceParameters *>(reinterpret_cast<unsigned char *>(current) + 0x14);
            }
            indexData += 0x30;
            count++;
        }
    }

    return count;
}

void GRaceDatabase::BuildRaceList() {
    unsigned int i;

    mRaceCountStatic = StoreRaceList(nullptr);
    mRaceCountDynamic = 0;
    mRaceIndex = reinterpret_cast<GRaceIndexData *>(bMalloc(mRaceCountStatic * 0x30, 0));
    mRaceParameters = static_cast<GRaceParameters *>(bMalloc(mRaceCountStatic * 0x14, 0));
    for (i = 0; i < 4; i++) {
        mRaceCustom[i] = nullptr;
    }
    StoreRaceList(mRaceParameters);
}

void GRaceDatabase::RefreshBinProgress() {
    unsigned int i;

    for (i = 0; i < mBinCount; i++) {
        mBins[i].RefreshProgress();
    }
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

GRaceParameters *GRaceDatabase::GetRaceFromHash(unsigned int hash) {
    unsigned int i;

    for (i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *race;

        race = GetRaceParameters(i);
        if (race && race->GetEventHash() == hash) {
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

    mStartupRaceContext = GRace::kRaceContext_QuickRace;
    mStartupRace = nullptr;
}

void GRaceDatabase::SetStartupRace(GRaceCustom *custom, GRace::Context context) {
    if (mStartupRace) {
        ClearStartupRace();
    }

    mStartupRaceContext = context;
    mStartupRace = custom;

    if (custom && context == GRace::kRaceContext_Career) {
        custom->SetupTimeOfDay();
    }
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

unsigned int GRaceDatabase::SerializeBins(unsigned char *dest) {
    unsigned int i;
    unsigned char *cursor;

    *reinterpret_cast<unsigned int *>(dest) = mBinCount;
    cursor = dest + 4;
    for (i = 0; i < mBinCount; i++) {
        unsigned short bytes;

        *reinterpret_cast<unsigned short *>(cursor) = static_cast<unsigned short>(mBins[i].GetBinNumber());
        bytes = static_cast<unsigned short>(mBins[i].Serialize(cursor + 4));
        *reinterpret_cast<unsigned short *>(cursor + 2) = bytes;
        cursor += 4 + bytes;
    }

    return cursor - dest;
}

unsigned int GRaceDatabase::DeserializeBins(unsigned char *src) {
    unsigned int count;
    unsigned int i;
    unsigned char *cursor;

    count = *reinterpret_cast<unsigned int *>(src);
    cursor = src + 4;
    for (i = 0; i < count; i++) {
        GRaceBin *bin;
        unsigned short bytes;

        bytes = *reinterpret_cast<unsigned short *>(cursor + 2);
        bin = GetBinNumber(*reinterpret_cast<unsigned short *>(cursor));
        if (bin) {
            bin->Deserialize(cursor + 4);
        }
        cursor += 4 + bytes;
    }

    return cursor - src;
}

GRaceParameters *GRaceDatabase::GetRaceFromActivity(GActivity *activity) {
    return GetRaceFromKey(activity->GetCollection());
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

GRaceCustom *GRaceDatabase::AllocCustomRace(GRaceParameters *parms) {
    GRaceCustom *custom;

    custom = nullptr;
    if (parms) {
        parms->BlockUntilLoaded();
        custom = new GRaceCustom(*parms);
        mRaceCustom[mRaceCountDynamic++] = custom;
    }

    return custom;
}

void GRaceDatabase::BuildScoreList() {
    unsigned int i;

    mRaceScoreInfo = static_cast<GRaceSaveInfo *>(bMalloc(mRaceCountStatic << 4, 0x800));
    if (!mRaceScoreInfo) {
        return;
    }

    bMemSet(mRaceScoreInfo, 0, mRaceCountStatic << 4);

    for (i = 0; i < mRaceCountStatic; i++) {
        GRaceParameters *race;

        race = &mRaceParameters[i];
        if (!race->GetIsDDayRace()) {
            bool unlockedQuick;
            bool unlockedOnline;
            bool unlockedChallenge;

            unlockedQuick = race->GetInitiallyUnlockedQuickRace();
            unlockedOnline = race->GetInitiallyUnlockedOnline();
            unlockedChallenge = race->GetInitiallyUnlockedChallenge();

            if (race->GetIsBossRace()) {
                unlockedQuick = false;
                unlockedOnline = false;
                unlockedChallenge = false;
            }

            if (unlockedQuick || unlockedOnline || unlockedChallenge) {
                unsigned int *flags;

                flags = &reinterpret_cast<unsigned int *>(GetScoreInfo(race->GetEventHash()))[1];
                if (unlockedQuick || unlockedChallenge) {
                    *flags |= kUnlocked_QuickRace;
                }
                if (unlockedOnline) {
                    *flags |= kUnlocked_Online;
                }
            }
        }
    }
}

void GRaceDatabase::ClearRaceScores() {
    unsigned int i;

    if (!mRaceScoreInfo) {
        return;
    }

    bMemSet(mRaceScoreInfo, 0, mRaceCountStatic << 4);
    for (i = 0; i < mRaceCountStatic; i++) {
        GRaceParameters *race;

        race = &mRaceParameters[i];
        if (!race->GetIsDDayRace()) {
            bool unlockedQuick;
            bool unlockedOnline;
            bool unlockedChallenge;

            unlockedQuick = race->GetInitiallyUnlockedQuickRace();
            unlockedOnline = race->GetInitiallyUnlockedOnline();
            unlockedChallenge = race->GetInitiallyUnlockedChallenge();

            if (race->GetIsBossRace()) {
                unlockedChallenge = false;
            }

            if (unlockedQuick || unlockedOnline || unlockedChallenge) {
                unsigned int *flags;

                flags = &reinterpret_cast<unsigned int *>(GetScoreInfo(race->GetEventHash()))[1];
                if (unlockedQuick || unlockedChallenge) {
                    *flags |= kUnlocked_QuickRace;
                }
                if (unlockedOnline) {
                    *flags |= kUnlocked_Online;
                }
            }
        }
    }
}

GRaceSaveInfo *GRaceDatabase::GetScoreInfo(unsigned int hash) {
    unsigned int i;
    int *scoreInfo;

    i = 0;
    scoreInfo = reinterpret_cast<int *>(mRaceScoreInfo);
    while (i < mRaceCountStatic && *scoreInfo) {
        if (static_cast<unsigned int>(*scoreInfo) == hash) {
            return reinterpret_cast<GRaceSaveInfo *>(scoreInfo);
        }
        i++;
        scoreInfo += 4;
    }

    *scoreInfo = static_cast<int>(hash);
    return reinterpret_cast<GRaceSaveInfo *>(scoreInfo);
}

bool GRaceDatabase::CheckRaceScoreFlags(unsigned int hash, ScoreFlags flags) {
    GRaceSaveInfo *scoreInfo;

    scoreInfo = GetScoreInfo(hash);
    return scoreInfo && (reinterpret_cast<unsigned int *>(scoreInfo)[1] & flags) != 0;
}

void GRaceDatabase::ResetCareerCompleteFlag(unsigned int hash) {
    unsigned int *flags;

    flags = &reinterpret_cast<unsigned int *>(GetScoreInfo(hash))[1];
    *flags &= ~kCompleted_ContextCareer;
}

void GRaceDatabase::LoadBestScores(GRaceSaveInfo *scores, unsigned int count) {
    int *scoreWords;
    int *dest;
    unsigned int loaded;
    unsigned int i;

    loaded = 0;
    i = 0;
    scoreWords = reinterpret_cast<int *>(scores);
    dest = reinterpret_cast<int *>(mRaceScoreInfo);
    bMemSet(mRaceScoreInfo, 0, mRaceCountStatic << 4);
    while (i < count && loaded != mRaceCountStatic) {
        if (scoreWords[0] && GetRaceFromHash(scoreWords[0])) {
            loaded++;
            bMemCpy(dest, scoreWords, 0x10);
            dest += 4;
        }
        scoreWords += 4;
        i++;
    }
}

const char *GRaceDatabase::GetNextDDayRace() {
    int i;

    for (i = 0; i < 5; i++) {
        GRaceParameters *race;

        race = GetRaceFromHash(Attrib::StringHash32(kDDayRaces[i]));
        if (!CheckRaceScoreFlags(race->GetEventHash(), kCompleted_ContextCareer)) {
            return kDDayRaces[i];
        }
    }

    return nullptr;
}

void GRaceDatabase::UpdateRaceScore(bool raceCompleted) {
    GRaceParameters *race;
    GRacerInfo *winner;
    int *scoreInfo;
    float recordValue;
    float value;
    int scale;

    race = GRaceStatus::Get().GetRaceParameters();
    winner = GRaceStatus::Get().GetWinningPlayerInfo();
    if (!winner || !race) {
        return;
    }

    scoreInfo = reinterpret_cast<int *>(GetScoreInfo(race->GetEventHash()));

    value = winner->GetTopSpeed();
    scale = 8;
    recordValue = static_cast<float>(*reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(scoreInfo) + 0x0C)) / scale;
    if (recordValue < value) {
        recordValue = value;
    }
    *reinterpret_cast<short *>(reinterpret_cast<char *>(scoreInfo) + 0x0C) = static_cast<short>(recordValue * scale);

    value = winner->GetDistDriven();
    recordValue = static_cast<float>(*reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(scoreInfo) + 0x0E)) / scale;
    if (recordValue < value) {
        recordValue = value;
    }
    *reinterpret_cast<short *>(reinterpret_cast<char *>(scoreInfo) + 0x0E) = static_cast<short>(recordValue * scale);

    switch (race->GetRaceType()) {
    case GRace::kRaceType_P2P:
    case GRace::kRaceType_Circuit:
    case GRace::kRaceType_Drag:
    case GRace::kRaceType_Knockout:
    case GRace::kRaceType_Tollbooth:
    case GRace::kRaceType_JumpToSpeedTrap:
    case GRace::kRaceType_JumpToMilestone:
        if ((scoreInfo[1] & 3) == 0 || winner->GetRaceTime() < *reinterpret_cast<float *>(scoreInfo + 2)) {
            *reinterpret_cast<float *>(scoreInfo + 2) = winner->GetRaceTime();
        }
        break;

    case GRace::kRaceType_SpeedTrap:
    case GRace::kRaceType_CashGrab:
        if ((scoreInfo[1] & 3) == 0 || *reinterpret_cast<float *>(scoreInfo + 2) < winner->GetPointTotal()) {
            *reinterpret_cast<float *>(scoreInfo + 2) = winner->GetPointTotal();
        }
        break;

    case GRace::kRaceType_Checkpoint:
        if ((scoreInfo[1] & 3) == 0 || static_cast<unsigned int>(scoreInfo[2]) < static_cast<unsigned int>(winner->GetPointTotal())) {
            scoreInfo[2] = static_cast<unsigned int>(winner->GetPointTotal());
        }
        break;

    default:
        break;
    }

    if (raceCompleted) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            scoreInfo[1] |= kCompleted_ContextCareer;
        } else {
            scoreInfo[1] |= kCompleted_ContextQuickRace;
        }
        RefreshBinProgress();
    }
}
