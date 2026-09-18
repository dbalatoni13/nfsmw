#include "GRaceDatabase.h"

#include "GRaceStatus.h"

#include "GActivity.h"
#include "GCharacter.h"
#include "GManager.h"
#include "GMarker.h"
#include "GMilestone.h"
#include "GObjectBlock.h"
#include "GSpeedTrap.h"
#include "GVault.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/FixedPoint.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/Misc/PackedDecimal.h"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay_hash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

inline PackedDecimal::PackedDecimal(float value) {
    int exponent = 0;
    int mantissa = 0;
    bool negative = false;

    if (value < 0.0f) {
        negative = true;
        value = -value;
    }
    if (value > 0.0f) {
        while (value < (float)FloatingPoint<short, 10, 3, 5, 11>::GetNormalizedLower()) {
            value *= 10.0f;
            exponent--;
        }
        mantissa = (int)value;
        while (mantissa >= FloatingPoint<short, 10, 3, 5, 11>::GetNormalizedUpper()) {
            mantissa /= 10;
            exponent++;
        }
    }
    if (negative) {
        mantissa = -mantissa;
    }

    mExp = exponent;
    mMan = mantissa;
}

class Minimap {
  public:
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
};

GRaceDatabase *GRaceDatabase::mObj = NULL;

const char GRaceDatabase::sDDayRaces[5][8] = {
    "16.1.0",
    "16.2.2",
    "16.2.3",
    "16.1.1",
    "16.2.1",
};

GRaceDatabase::GRaceDatabase() {
    mRaceCountStatic = 0;
    mRaceCountDynamic = 0;
    mRaceIndex = NULL;
    mRaceParameters = NULL;
    mBinCount = 0;
    mBins = NULL;
    mGameplayClass = Attrib::Database::Get().GetClass(Attrib::ClassName::gameplay);
    mStartupRace = NULL;
    mStartupRaceContext = kRaceContext_QuickRace;
    mRaceScoreInfo = NULL;

    DVDErrorTask(NULL, 0);
    BuildBinList();
    DVDErrorTask(NULL, 0);
    BuildRaceList();
    DVDErrorTask(NULL, 0);
    BuildScoreList();
    DVDErrorTask(NULL, 0);
}

void GRaceDatabase::Init() {
    mObj = new ("GRaceDatabase", 0) GRaceDatabase;
}

void GRaceDatabase::BuildBinList() {
    mBinCount = StoreBinList(NULL);
    mBins = (GRaceBin *)::operator new[](mBinCount * sizeof(GRaceBin), "Gameplay race bin list", 0);
    StoreBinList(mBins);
}

unsigned int GRaceDatabase::StoreBinList(GRaceBin *list) {
    unsigned int count = 0;

    Attrib::Key collectionKey = mGameplayClass->GetFirstCollection();

    while (collectionKey != 0) {
        Attrib::Gen::gameplay instanceObj(collectionKey, 0, NULL);

        if (CollectionIsRaceBin(instanceObj)) {
            if (list != NULL) {
                new (&list[count]) GRaceBin(collectionKey);
            }
            count++;
        }

        collectionKey = mGameplayClass->GetNextCollection(collectionKey);
    }

    return count;
}

unsigned int GRaceDatabase::SerializeBins(unsigned char *dest) {
    unsigned char *start = dest;

    *(unsigned int *)dest = mBinCount;
    dest += 4;

    for (unsigned int i = 0; i < mBinCount; i++) {
        *(unsigned short *)dest = (unsigned short)mBins[i].GetBinNumber();
        dest += 2;

        unsigned short *lenSlot = (unsigned short *)dest;
        dest += 2;

        *lenSlot = mBins[i].Serialize(dest);
        dest += *lenSlot;
    }

    return dest - start;
}

unsigned int GRaceDatabase::DeserializeBins(unsigned char *src) {
    unsigned char *start = src;
    unsigned int count = *(unsigned int *)src;

    src += 4;

    for (unsigned int i = 0; i < count; i++) {
        int binNumber = *(unsigned short *)src;
        src += 2;

        unsigned short size = *(unsigned short *)src;
        src += 2;

        GRaceBin *bin = GetBinNumber(binNumber);
        if (bin != NULL) {
            bin->Deserialize(src);
        }

        src += size;
    }

    return src - start;
}

void GRaceDatabase::RefreshBinProgress() {
    for (unsigned int i = 0; i < mBinCount; i++) {
        mBins[i].RefreshProgress();
    }
}

void GRaceDatabase::BuildRaceList() {
    unsigned int raceCount = StoreRaceList(NULL);
    mRaceCountStatic = raceCount;
    mRaceCountDynamic = 0;

    unsigned int indexAllocParams = GetVirtualMemoryAllocParams();
    mRaceIndex = (GRaceIndexData *)bMalloc(raceCount * sizeof(GRaceIndexData), indexAllocParams);

    unsigned int parmsCount = mRaceCountStatic;
    unsigned int parmsAllocParams = GetVirtualMemoryAllocParams();
    mRaceParameters = (GRaceParameters *)bMalloc(parmsCount * sizeof(GRaceParameters), parmsAllocParams);

    for (unsigned int i = 0; i < 4; i++) {
        mRaceCustom[i] = NULL;
    }

    StoreRaceList(mRaceParameters);
}

unsigned int GRaceDatabase::StoreRaceList(GRaceParameters *list) {
    unsigned int count = 0;

    Attrib::Key collectionKey = mGameplayClass->GetFirstCollection();

    while (collectionKey != 0) {
        DVDErrorTask(NULL, 0);

        Attrib::Gen::gameplay instanceObj(collectionKey, 0, NULL);

        if (CollectionIsRaceActivity(instanceObj)) {
            if (list != NULL) {
                new (&list[count]) GRaceParameters(collectionKey, &mRaceIndex[count]);
            }
            count++;
        }

        collectionKey = mGameplayClass->GetNextCollection(collectionKey);
    }

    return count;
}

bool GRaceDatabase::CollectionIsRaceActivity(Attrib::Gen::gameplay &instanceObj) {
    Attrib::Gen::gameplay raceObj(Attrib::Hash::gameplay::key_race, 0, NULL);

    if (instanceObj.Template()) {
        return false;
    }

    unsigned int parentKey = instanceObj.GetParent();

    while (parentKey != 0) {
        Attrib::Gen::gameplay parentObj(parentKey, 0, 0);

        if (parentObj.GetCollection() == raceObj.GetCollection()) {
            return true;
        }

        parentKey = parentObj.GetParent();
    }

    return false;
}

bool GRaceDatabase::CollectionIsRaceBin(Attrib::Gen::gameplay &instanceObj) {
    unsigned int parentKey = instanceObj.GetParent();

    Attrib::Gen::gameplay templateObj(Attrib::Hash::gameplay::key_race_bin, 0, 0);
    Attrib::Gen::gameplay parentObj(parentKey, 0, 0);

    if (parentObj.GetCollection() == templateObj.GetCollection()) {
        return true;
    }

    return false;
}

void GRaceDatabase::NotifyVaultUnloading(GVault *vault) {
    for (unsigned int i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *parms = GetRaceParameters(i);

        if (parms->GetParentVault() == vault) {
            parms->NotifyParentVaultUnloading();
        }
    }

    if (mStartupRace != NULL && vault == mStartupRace->GetParentVault()) {
        ClearStartupRace();
    }
}

void GRaceDatabase::NotifyVaultLoaded(GVault *vault) {
    for (unsigned int i = 0; i < GetRaceCount(); i++) {
        GRaceParameters *parms = GetRaceParameters(i);

        if (parms->GetParentVault() == vault) {
            parms->NotifyParentVaultLoaded();
        }
    }
}

unsigned int GRaceDatabase::GetRaceCount() {
    return mRaceCountStatic + mRaceCountDynamic;
}

GRaceParameters *GRaceDatabase::GetRaceParameters(unsigned int index) {
    if (index < mRaceCountStatic) {
        return &mRaceParameters[index];
    }

    index -= mRaceCountStatic;

    return mRaceCustom[index];
}

GRaceParameters *GRaceDatabase::GetRaceFromActivity(GActivity *activity) {
    return GetRaceFromKey(activity->GetCollection());
}

GRaceParameters *GRaceDatabase::GetRaceFromHash(unsigned int hash) {
    for (unsigned int onRace = 0; onRace < GetRaceCount(); onRace++) {
        GRaceParameters *parms = GetRaceParameters(onRace);

        if (parms->GetEventHash() == hash) {
            return parms;
        }
    }

    return nullptr;
}

GRaceParameters *GRaceDatabase::GetRaceFromKey(unsigned int collectionKey) {
    for (unsigned int onRace = 0; onRace < GetRaceCount(); onRace++) {
        GRaceParameters *parms = GetRaceParameters(onRace);

        if (parms->GetCollectionKey() == collectionKey) {
            return parms;
        }
    }

    return nullptr;
}

unsigned int GRaceDatabase::GetBinCount() {
    return mBinCount;
}

GRaceBin *GRaceDatabase::GetBin(unsigned int index) {
    return &mBins[index];
}

GRaceBin *GRaceDatabase::GetBinNumber(int number) {
    for (unsigned int onBin = 0; onBin < GetBinCount(); onBin++) {
        if (mBins[onBin].GetBinNumber() == number) {
            return &mBins[onBin];
        }
    }

    return nullptr;
}

GRaceCustom *GRaceDatabase::AllocCustomRace(GRaceParameters *parms) {
    if (parms == NULL) {
        return NULL;
    }

    parms->BlockUntilLoaded();

    unsigned int slot = mRaceCountDynamic++;

    return mRaceCustom[slot] = new GRaceCustom(*parms);
}

void GRaceDatabase::FreeCustomRace(GRaceCustom *custom) {
    if (custom == NULL) {
        return;
    }

    if (custom == mStartupRace) {
        custom->SetFreedByOwner();
    } else {
        DestroyCustomRace(custom);
    }
}

void GRaceDatabase::DestroyCustomRace(GRaceCustom *custom) {
    if (custom != NULL) {
        delete custom;
    }

    for (unsigned int i = 0; i < 4; i++) {
        if (mRaceCustom[i] == custom) {
            if (i < mRaceCountDynamic - 1) {
                mRaceCustom[i] = mRaceCustom[mRaceCountDynamic - 1];
            }
            mRaceCountDynamic--;
            mRaceCustom[mRaceCountDynamic] = NULL;
            break;
        }
    }
}

void GRaceDatabase::ClearStartupRace() {
    if (mStartupRace != NULL && mStartupRace->GetFreedByOwner()) {
        DestroyCustomRace(mStartupRace);
    }

    mStartupRace = NULL;
    mStartupRaceContext = kRaceContext_QuickRace;
}

void GRaceDatabase::SetStartupRace(GRaceCustom *custom, GRace::Context context) {
    if (mStartupRace != NULL) {
        ClearStartupRace();
    }

    mStartupRace = custom;
    mStartupRaceContext = (Context)context;

    if (custom != NULL && context == GRace::kRaceContext_Career) {
        custom->SetupTimeOfDay();
    }
}

GRaceCustom *GRaceDatabase::GetStartupRace() {
    return mStartupRace;
}

Context GRaceDatabase::GetStartupRaceContext() {
    return mStartupRaceContext;
}

void GRaceDatabase::BuildScoreList() {
    mRaceScoreInfo = (GRaceSaveInfo *)bMalloc(mRaceCountStatic * sizeof(GRaceSaveInfo), 0x800);
    ClearRaceScores();
}

void GRaceDatabase::UpdateRaceScore(bool setComplete) {
    GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
    unsigned int eventHash = raceParms->GetEventHash();
    GRacerInfo *racer = GRaceStatus::Get().GetWinningPlayerInfo();

    if (racer == NULL) {
        return;
    }

    GRaceSaveInfo *info = GetScoreInfo(eventHash);

    float topSpeed = racer->mTopSpeed;

    info->mTopSpeed = UMath::Max(topSpeed, (float)info->mTopSpeed);
    info->mAverageSpeed = UMath::Max(racer->CalcAverageSpeed(), (float)info->mAverageSpeed);

    switch (raceParms->GetRaceType()) {
    case GRace::kRaceType_Circuit:
    case GRace::kRaceType_Knockout:
        if ((info->mFlags & kCompleted_ContextAny) == 0
            || GRaceStatus::Get().GetBestLapTime(racer->mIndex) < info->mHighScores.mBestTime) {
            info->mHighScores.mBestTime = GRaceStatus::Get().GetBestLapTime(racer->mIndex);
        }
        break;

    case GRace::kRaceType_P2P:
    case GRace::kRaceType_Drag:
    case GRace::kRaceType_Tollbooth:
    case GRace::kRaceType_JumpToSpeedTrap:
    case GRace::kRaceType_JumpToMilestone:
        if ((info->mFlags & kCompleted_ContextAny) == 0 || racer->GetRaceTime() < info->mHighScores.mBestTime) {
            info->mHighScores.mBestTime = racer->GetRaceTime();
        }
        break;

    case GRace::kRaceType_Checkpoint: {
        unsigned int points = (unsigned int)racer->mPointTotal;

        if ((info->mFlags & kCompleted_ContextAny) == 0 || points > info->mHighScores.mBestPoints) {
            info->mHighScores.mBestPoints = points;
        }
        break;
    }

    case GRace::kRaceType_SpeedTrap:
    case GRace::kRaceType_CashGrab:
        if ((info->mFlags & kCompleted_ContextAny) == 0 || racer->mPointTotal > info->mHighScores.mBestCash) {
            info->mHighScores.mBestCash = racer->mPointTotal;
        }
        break;

    default:
        break;
    }

    if (setComplete) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            info->mFlags |= kCompleted_ContextCareer;
        } else {
            info->mFlags |= kCompleted_ContextQuickRace;
        }

        RefreshBinProgress();
    }
}

void GRaceDatabase::ClearRaceScores() {
    if (mRaceScoreInfo != NULL) {
        bMemSet(mRaceScoreInfo, 0, mRaceCountStatic * sizeof(GRaceSaveInfo));

        for (unsigned int i = 0; i < mRaceCountStatic; i++) {
            GRaceParameters *parms = &mRaceParameters[i];

            if (parms->GetIsDDayRace()) {
                continue;
            }

            bool quickUnlocked = parms->GetInitiallyUnlockedQuickRace();
            bool onlineUnlocked = parms->GetInitiallyUnlockedOnline();
            bool challengeUnlocked = parms->GetInitiallyUnlockedChallenge();

            if (parms->GetIsBossRace()) {
                challengeUnlocked = false;
            }

            if (quickUnlocked || onlineUnlocked || challengeUnlocked) {
                GRaceSaveInfo *info = GetScoreInfo(parms->GetEventHash());

                if (quickUnlocked || challengeUnlocked) {
                    info->mFlags |= kUnlocked_QuickRace;
                }
                if (onlineUnlocked) {
                    info->mFlags |= kUnlocked_Online;
                }
            }
        }
    }
}

GRaceSaveInfo *GRaceDatabase::GetScoreInfo(unsigned int eventHash) {
    unsigned int index = 0;

    GRaceSaveInfo *current = mRaceScoreInfo;

    for (index = 0; index < mRaceCountStatic; index++) {
        if (current->mRaceHash == 0) {
            break;
        }
        if (current->mRaceHash == eventHash) {
            return current;
        }
        current++;
    }

    current->mRaceHash = eventHash;
    return current;
}

bool GRaceDatabase::CheckRaceScoreFlags(unsigned int eventHash, ScoreFlags mask) {
    GRaceSaveInfo *info = GetScoreInfo(eventHash);

    if (info != nullptr) {
        return (info->mFlags & mask) != 0;
    }

    return false;
}

void GRaceDatabase::ResetCareerCompleteFlag(unsigned int eventHash) {
    GRaceSaveInfo *info = GetScoreInfo(eventHash);

    info->mFlags &= ~kCompleted_ContextCareer;
}

void GRaceDatabase::LoadBestScores(GRaceSaveInfo *entries, unsigned int numEntries) {
    bMemSet(mRaceScoreInfo, 0, mRaceCountStatic * sizeof(GRaceSaveInfo));

    unsigned int numLoaded = 0;
    GRaceSaveInfo *dst = mRaceScoreInfo;

    for (unsigned int i = 0; i < numEntries && numLoaded != mRaceCountStatic; i++) {
        if (entries->mRaceHash != 0 && GetRaceFromHash(entries->mRaceHash) != NULL) {
            bMemCpy(dst, entries, sizeof(GRaceSaveInfo));
            numLoaded++;
            dst++;
        }

        entries++;
    }
}

const char *GRaceDatabase::GetNextDDayRace() {
    for (int i = 0; i < 5; i++) {
        const char *raceName = sDDayRaces[i];
        GRaceParameters *race = GetRaceFromHash(Attrib::StringHash32(raceName));

        if (!CheckRaceScoreFlags(race->GetEventHash(), kCompleted_ContextCareer)) {
            return raceName;
        }
    }

    return NULL;
}

unsigned int GRaceParameters::GetCollectionKey() const {
    if (mIndex != nullptr) {
        return mIndex->mKey;
    }
    EnsureLoaded();
    return mRaceRecord->GetCollection();
}

float GRaceParameters::GetRaceLengthMeters() const {
    if (mIndex != nullptr) {
        return mIndex->mLength;
    }
    EnsureLoaded();
    return mRaceRecord->RaceLength();
}

int GRaceParameters::GetReputation() const {
    if (mIndex != nullptr) {
        return (int)(float)mIndex->mReputation;
    }
    EnsureLoaded();
    return mRaceRecord->Reputation();
}

float GRaceParameters::GetCashValue() const {
    if (mIndex != nullptr) {
        return (float)mIndex->mCash;
    }
    EnsureLoaded();
    return mRaceRecord->CashValue();
}

int GRaceParameters::GetLocalizationTag() const {
    if (mIndex != nullptr) {
        return mIndex->mLocalizationTag;
    }
    EnsureLoaded();
    return mRaceRecord->LocalizationTag();
}

int GRaceParameters::GetNumLaps() const {
    if (mIndex != nullptr) {
        return mIndex->mNumLaps;
    }
    EnsureLoaded();
    return mRaceRecord->NumLaps();
}

const char *GRaceParameters::GetEventID() const {
    if (mIndex != nullptr) {
        return mIndex->mEventID;
    }
    EnsureLoaded();
    return mRaceRecord->EventID();
}

float GRaceParameters::GetRivalBestTime() const {
    if (mIndex != nullptr) {
        return (float)mIndex->mRivalBest;
    }
    EnsureLoaded();
    return mRaceRecord->RivalBestTime();
}

float GRaceParameters::GetChallengeGoal() const {
    if (mIndex != nullptr) {
        return (float)mIndex->mChallengeGoal;
    }
    EnsureLoaded();
    return mRaceRecord->ThreshholdValue();
}

bool GRaceParameters::GetInitiallyUnlockedQuickRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_AvailableQR) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->AvailableQR();
}

bool GRaceParameters::GetInitiallyUnlockedOnline() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_AvailableOnline) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->AvailableOnline();
}

bool GRaceParameters::GetInitiallyUnlockedChallenge() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_InitiallyUnlocked) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->InitiallyUnlocked();
}

bool GRaceParameters::GetIsDDayRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_DDayRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->DDayRace();
}

bool GRaceParameters::GetIsBossRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_BossRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->BossRace();
}

bool GRaceParameters::GetIsMarkerRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_MarkerRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->IsMarkerRace();
}

bool GRaceParameters::GetIsPursuitRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_PursuitRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->PursuitRace();
}

bool GRaceParameters::GetIsLoopingRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_LoopingRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->IsLoopingRace();
}

bool GRaceParameters::GetRankPlayersByPoints() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_RankPlayersByPoints) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->RankPlayersByPoints();
}

bool GRaceParameters::GetRankPlayersByDistance() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_RankPlayersByDistance) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->RankPlayersByDistance();
}

bool GRaceParameters::GetCopsEnabled() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_CopsInRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->CopsInRace();
}

bool GRaceParameters::GetScriptedCopsInRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_ScriptedCopsInRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->ScriptedCopsInRace();
}

bool GRaceParameters::GetNeverInQuickRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_NeverInQuickRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->NeverInQuickRace();
}

bool GRaceParameters::GetIsChallengeSeriesRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_ChallengeSeriesRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->ChallengeSeriesRace();
}

bool GRaceParameters::GetIsCollectorsEditionRace() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_CollectorsEditionRace) != 0;
    }
    EnsureLoaded();
    return mRaceRecord->CollectorsEditionRace();
}

float GRaceParameters::GetTimeLimit() const {
    EnsureLoaded();
    return mRaceRecord->TimeLimit();
}

float GRaceParameters::GetMaxHeatLevel() const {
    EnsureLoaded();
    return mRaceRecord->MaxHeatLevel();
}

bool GRaceParameters::GetNoPostRaceScreen() const {
    EnsureLoaded();
    return mRaceRecord->NoPostRaceScreen();
}

bool GRaceParameters::GetUseWorldHeatInRace() const {
    EnsureLoaded();
    return mRaceRecord->UseWorldHeat();
}

float GRaceParameters::GetForceHeatLevel() const {
    EnsureLoaded();
    return mRaceRecord->ForceHeatLevel();
}

float GRaceParameters::GetMaxRaceHeatLevel() const {
    EnsureLoaded();
    return mRaceRecord->MaxHeatLevel();
}

float GRaceParameters::GetInitialPlayerSpeed() const {
    EnsureLoaded();
    return mRaceRecord->InitialPlayerSpeed();
}

bool GRaceParameters::GetIsRollingStart() const {
    EnsureLoaded();
    return mRaceRecord->RollingStart();
}

bool GRaceParameters::GetIsEpicPursuitRace() const {
    EnsureLoaded();
    return mRaceRecord->IsEpicPursuitRace();
}

const char *GRaceParameters::GetPlayerCarType() const {
    EnsureLoaded();
    return mRaceRecord->PlayerCarType();
}

float GRaceParameters::GetPlayerCarPerformance() const {
    EnsureLoaded();
    return mRaceRecord->PlayerCarPerformance();
}

int GRaceParameters::GetKnockoutsPerLap() const {
    EnsureLoaded();
    return mRaceRecord->KnockoutsPerLap();
}

bool GRaceParameters::GetCatchUp() const {
    EnsureLoaded();
    return mRaceRecord->CatchUp();
}

bool GRaceParameters::GetCatchUpOverride() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpOverride();
}

const char *GRaceParameters::GetCatchUpSkill() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpSkill();
}

const char *GRaceParameters::GetCatchUpSpread() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpSpread();
}

float GRaceParameters::GetCatchUpIntegral() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpIntegral();
}

float GRaceParameters::GetCatchUpDerivative() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpDerivative();
}

unsigned int GRaceParameters::GetNumCheckpoints() const {
    EnsureLoaded();
    return mRaceRecord->Num_Checkpoint();
}

bool GRaceParameters::GetCheckpointsVisible() const {
    EnsureLoaded();
    return mRaceRecord->CheckpointsVisible();
}

unsigned int GRaceParameters::GetNumShortcuts() const {
    EnsureLoaded();
    return mRaceRecord->Num_Shortcuts();
}

unsigned int GRaceParameters::GetNumBarrierExemptions() const {
    EnsureLoaded();
    return mRaceRecord->Num_BarrierExemptions();
}

unsigned int GRaceParameters::GetBarrierCount() const {
    EnsureLoaded();
    return mRaceRecord->Num_Barriers();
}

const char *GRaceParameters::GetTrafficPattern() const {
    EnsureLoaded();
    return mRaceRecord->TrafficPattern();
}

const char *GRaceParameters::GetPhotoFinishCamera() const {
    EnsureLoaded();
    return mRaceRecord->FinishCamera();
}

const char *GRaceParameters::GetPhotoFinishTexture() const {
    EnsureLoaded();
    return mRaceRecord->PostRaceScreenTexture();
}

float GRaceParameters::GetTimeOfDay() const {
    EnsureLoaded();

    return mRaceRecord->TOD();
}

float GRaceParameters::GetStartTime() const {
    EnsureLoaded();
    return mRaceRecord->StartTime();
}

float GRaceParameters::GetStartPercent() const {
    EnsureLoaded();
    return mRaceRecord->StartPercent();
}

inline void GRaceParameters::EnsureLoaded() const {
    if (mParentVault != nullptr && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }
    if (mChildVault != nullptr && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }
}

void GRaceParameters::BlockUntilLoaded() {
    if (mParentVault != nullptr && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }
    if (mChildVault != nullptr && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }
}

bool GRaceParameters::GetIsLoaded() const {
    if (mParentVault != nullptr && !mParentVault->IsLoaded()) {
        return false;
    }
    if (mChildVault != nullptr && !mChildVault->IsLoaded()) {
        return false;
    }
    return true;
}

GRaceParameters::GRaceParameters(unsigned int collectionKey, GRaceIndexData *index)
    : mIndex(NULL), mRaceRecord(new Attrib::Gen::gameplay(collectionKey, 0, NULL)), mParentVault(NULL), mChildVault(NULL) {
    GenerateIndex(index);
    mIndex = index;

    const char *vaultName = mRaceRecord->gameplayvault();
    if (vaultName != NULL) {
        mChildVault = GManager::Get().FindVault(vaultName);
        mRaceRecord->Num_Children();
    }

    mParentVault = GManager::Get().FindVaultContaining(mRaceRecord->GetCollection());
    if (mParentVault != NULL) {
        mParentVault->IsTransient();
    }
}

GRaceParameters::~GRaceParameters() {
    if (mRaceRecord != NULL) {
        delete mRaceRecord;
        mRaceRecord = NULL;
    }

    mIndex = NULL;
}

void GRaceParameters::GenerateIndex(GRaceIndexData *index) {
    if (index == nullptr) {
        return;
    }

    index->mKey = GetCollectionKey();
    index->mRaceHash = GetEventHash();
    index->mLocalizationTag = GetLocalizationTag();
    index->mLength = GetRaceLengthMeters();
    index->mRivalBest = FixedPoint<unsigned short, 10, 2>(GetRivalBestTime());
    index->mReputation = PackedDecimal(GetReputation());
    index->mCash = PackedDecimal(GetCashValue());
    index->mChallengeType = GetChallengeType();
    index->mChallengeGoal = PackedDecimal(GetChallengeGoal());
    index->mNumLaps = GetNumLaps();
    index->mCopDensity = GetCopDensity();
    index->mRaceType = GetRaceType();
    index->mRegion = GetRegion();

    index->mFlags = 0;

    unsigned int *pflags = &index->mFlags;
    unsigned int flags = 0;
    if (GetInitiallyUnlockedQuickRace()) {
        flags |= GRaceIndexData::kRaceFlag_AvailableQR;
    }
    index->mFlags = flags;
    if (GetInitiallyUnlockedOnline()) {
        flags |= GRaceIndexData::kRaceFlag_AvailableOnline;
    }
    index->mFlags = flags;
    if (GetInitiallyUnlockedChallenge()) {
        flags |= GRaceIndexData::kRaceFlag_InitiallyUnlocked;
    }
    index->mFlags = flags;
    if (GetCanBeReversed()) {
        flags |= GRaceIndexData::kRaceFlag_CanBeReversed;
    }
    index->mFlags = flags;
    if (GetIsDDayRace()) {
        flags |= GRaceIndexData::kRaceFlag_DDayRace;
    }
    index->mFlags = flags;
    if (GetIsBossRace()) {
        flags |= GRaceIndexData::kRaceFlag_BossRace;
    }
    index->mFlags = flags;
    if (GetIsMarkerRace()) {
        flags |= GRaceIndexData::kRaceFlag_MarkerRace;
    }
    index->mFlags = flags;
    if (GetIsPursuitRace()) {
        flags |= GRaceIndexData::kRaceFlag_PursuitRace;
    }
    index->mFlags = flags;
    if (GetIsLoopingRace()) {
        flags |= GRaceIndexData::kRaceFlag_LoopingRace;
    }
    index->mFlags = flags;
    bool rank = GetRankPlayersByPoints();
    unsigned int f = flags;
    if (rank) {
        f |= GRaceIndexData::kRaceFlag_RankPlayersByPoints;
    }
    *pflags = f;
    flags = index->mFlags;
    if (GetRankPlayersByDistance()) {
        flags |= GRaceIndexData::kRaceFlag_RankPlayersByDistance;
    }
    index->mFlags = flags;
    if (GetCopsEnabled()) {
        flags |= GRaceIndexData::kRaceFlag_CopsInRace;
    }
    index->mFlags = flags;
    if (GetScriptedCopsInRace()) {
        flags |= GRaceIndexData::kRaceFlag_ScriptedCopsInRace;
    }
    index->mFlags = flags;
    if (GetTimeOfDay() > 0.8f) {
        flags |= GRaceIndexData::kRaceFlag_Unknown13;
    }
    index->mFlags = flags;
    if (GetNeverInQuickRace()) {
        flags |= GRaceIndexData::kRaceFlag_NeverInQuickRace;
    }
    index->mFlags = flags;
    if (GetIsChallengeSeriesRace()) {
        flags |= GRaceIndexData::kRaceFlag_ChallengeSeriesRace;
    }
    index->mFlags = flags;
    if (GetIsCollectorsEditionRace()) {
        flags |= GRaceIndexData::kRaceFlag_CollectorsEditionRace;
    }
    index->mFlags = flags;

    if (GetTimeOfDay() >= 0.8f) {
        index->mFlags |= GRaceIndexData::kRaceFlag_Unknown13;
    } else if (GetTimeOfDay() >= 0.0f) {
        index->mFlags |= 0x20000;
    }

    bMemSet(index->mEventID, 0, sizeof(index->mEventID));
    bSafeStrCpy(index->mEventID, GetEventID(), sizeof(index->mEventID));

    UMath::Vector2 topLeft;
    UMath::Vector2 botRight;
    GetBoundingBox(topLeft, botRight);

    index->mMapX1 = (unsigned char)(topLeft.x * 255.0f);
    index->mMapY1 = (unsigned char)(topLeft.y * 255.0f);
    index->mMapX2 = (unsigned char)(botRight.x * 255.0f);
    index->mMapY2 = (unsigned char)(botRight.y * 255.0f);
}

void GRaceParameters::NotifyParentVaultUnloading() {
    if (mRaceRecord != NULL) {
        delete mRaceRecord;
        mRaceRecord = NULL;
    }
}

void GRaceParameters::NotifyParentVaultLoaded() {
    if (mRaceRecord == NULL) {
        mRaceRecord = new Attrib::Gen::gameplay(mIndex->mKey, 0, NULL);
    }
}

const Attrib::Gen::gameplay *GRaceParameters::GetGameplayObj() const {
    return mRaceRecord;
}

GActivity *GRaceParameters::GetActivity() const {
    return GRuntimeInstance::FindObject<GActivity>(GetCollectionKey());
}

GVault *GRaceParameters::GetChildVault() const {
    return mChildVault;
}

GVault *GRaceParameters::GetParentVault() const {
    return mParentVault;
}

void GRaceParameters::GetBoundingBox(UMath::Vector2 &topLeft, UMath::Vector2 &botRight) const {
    if (mIndex != nullptr) {
        topLeft.x = mIndex->mMapX1 * (1.0f / 255.0f);
        topLeft.y = mIndex->mMapY1 * (1.0f / 255.0f);
        botRight.x = mIndex->mMapX2 * (1.0f / 255.0f);
        botRight.y = mIndex->mMapY2 * (1.0f / 255.0f);
        return;
    }

    EnsureLoaded();

    UMath::Vector3 pos;

    GetStartPosition(pos);

    float minX = pos.z;
    float maxX = pos.z;
    float minY = -pos.x;
    float maxY = -pos.x;

    if (HasFinishLine()) {
        GetFinishPosition(pos);

        minX = std::min(minX, pos.z);
        maxX = std::max(maxX, pos.z);
        minY = std::min(minY, -pos.x);
        maxY = std::max(maxY, -pos.x);
    }

    for (unsigned int i = 0; i < GetNumCheckpoints(); i++) {
        GetCheckpointPosition(i, pos);

        minX = std::min(minX, pos.z);
        maxX = std::max(maxX, pos.z);
        minY = std::min(minY, -pos.x);
        maxY = std::max(maxY, -pos.x);
    }

    TrackInfo *track = TrackInfo::GetTrackInfo(2000);

    bVector2 worldMin(minX, minY);
    bVector2 worldMax(maxX, maxY);
    bVector2 mapMin;
    bVector2 mapMax;

    Minimap::ConvertPos(worldMin, mapMin, track);
    Minimap::ConvertPos(worldMax, mapMax, track);

    topLeft.x = mapMin.x;
    topLeft.y = mapMin.y;
    botRight.x = mapMax.x;
    botRight.y = mapMax.y;
}

unsigned int GRaceParameters::GetChallengeType() const {
    if (mIndex != nullptr) {
        return mIndex->mChallengeType;
    }

    EnsureLoaded();

    const char *name = mRaceRecord->MilestoneName();

    if (name == nullptr || name[0] == 0) {
        return 0;
    }

    return Attrib::StringHash32(name);
}

GRace::Type GRaceParameters::GetRaceType() const {
    if (mIndex != nullptr) {
        return (GRace::Type)mIndex->mRaceType;
    }

    EnsureLoaded();

    static struct {
        const char *mName;
        GRace::Type mType;
    } typeTable[] = {
        {"circuit", GRace::kRaceType_Circuit},
        {"p2p", GRace::kRaceType_P2P},
        {"drag", GRace::kRaceType_Drag},
        {"knockout", GRace::kRaceType_Knockout},
        {"tollbooth", GRace::kRaceType_Tollbooth},
        {"speedtrap", GRace::kRaceType_SpeedTrap},
        {"cashgrab", GRace::kRaceType_CashGrab},
        {"checkpointrace", GRace::kRaceType_Checkpoint},
        {"challenge", GRace::kRaceType_Challenge},
        {"speedtrapjump", GRace::kRaceType_JumpToSpeedTrap},
        {"milestonejump", GRace::kRaceType_JumpToMilestone},
    };

    const char *name = mRaceRecord->EventIconType();

    for (int i = 0; i <= 10; i++) {
        if (bStrCmp(name, typeTable[i].mName) == 0) {
            return typeTable[i].mType;
        }
    }

    return GRace::kRaceType_None;
}

GRace::Region GRaceParameters::GetRegion() const {
    if (mIndex != nullptr) {
        return (GRace::Region)mIndex->mRegion;
    }

    EnsureLoaded();

    static struct {
        const char *mName;
        GRace::Region mRegion;
    } regionTable[] = {
        {"college", GRace::kRaceRegion_College},
        {"coastal", GRace::kRaceRegion_Coastal},
        {"city", GRace::kRaceRegion_City},
    };

    const char *name = mRaceRecord->Region();

    for (int i = 0; i <= 2; i++) {
        if (bStrCmp(name, regionTable[i].mName) == 0) {
            return regionTable[i].mRegion;
        }
    }

    return GRace::kRaceRegion_None;
}

void GRaceParameters::ExtractPosition(Attrib::Gen::gameplay &collection, UMath::Vector3 &pos) const {
    const UMath::Vector3 &position = collection.Position();

    UMath::Vector3 posSwizzled = UMath::Vector3Make(-position.y, position.z, position.x);

    pos = posSwizzled;
}

void GRaceParameters::ExtractDirection(Attrib::Gen::gameplay &collection, UMath::Vector3 &dir, float rotate) const {
    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 initialVec = {0.0f, 0.0f, 1.0f};

    UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
    MATRIX4_multyrot(&rotMat, -(collection.Rotation() + rotate) / 360.0f, &rotMat);
    UMath::Rotate(initialVec, rotMat, initialVec);

    dir = initialVec;
}

unsigned int GRaceParameters::GetEventHash() const {
    if (mIndex != nullptr) {
        return mIndex->mRaceHash;
    }

    EnsureLoaded();

    return Attrib::StringHash32(mRaceRecord->EventID());
}

bool GRaceParameters::GetIsAvailable(GRace::Context context) const {
    if (UnlockAllThings) {
        return true;
    }

    switch (context) {
    case kRaceContext_Career: {
        bool isAvailable = false;

        if (GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_Career)) {
            isAvailable = GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kCompleted_ContextCareer) == 0;
        }

        return isAvailable;
    }
    case kRaceContext_QuickRace:
        if (GetNeverInQuickRace()) {
            return false;
        }
        return GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_QuickRace);
    case kRaceContext_Online:
        return GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_Online);
    default:
        return false;
    }
}

bool GRaceParameters::GetIsSunsetRace() const {
    if (mIndex != NULL) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_Unknown13) != 0;
    }

    if (mParentVault != NULL && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault != NULL && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    return GetTimeOfDay() >= 0.8f;
}

bool GRaceParameters::GetIsMiddayRace() const {
    if (mIndex != NULL) {
        return (mIndex->mFlags & 0x20000) != 0;
    }

    if (mParentVault != NULL && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault != NULL && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    return GetTimeOfDay() >= 0.0f && GetTimeOfDay() < 0.8f;
}

void GRaceParameters::SetupTimeOfDay() {
    if (GetIsSunsetRace()) {
        SetCurrentTimeOfDay(0.9f);
    } else if (GetIsMiddayRace()) {
        SetCurrentTimeOfDay(0.0f);
    }
}

int GRaceParameters::GetTrafficDensity() const {
    EnsureLoaded();

    return mRaceRecord->TrafficLevel();
}

GRace::Difficulty GRaceParameters::GetDifficulty() const {
    EnsureLoaded();

    int level = mRaceRecord->DifficultyLevel();

    if (level <= 33) {
        return GRace::kRaceDifficulty_Easy;
    } else if (level <= 66) {
        return GRace::kRaceDifficulty_Medium;
    }

    return GRace::kRaceDifficulty_Hard;
}

int GRaceParameters::GetCopDensity() const {
    if (mIndex != nullptr) {
        return mIndex->mCopDensity;
    }

    EnsureLoaded();

    int density = mRaceRecord->CopDensity();

    if (density == 0 && !GetCopsEnabled()) {
        return 0;
    }

    if (density <= 33) {
        return 1;
    }

    if (density <= 66) {
        return 2;
    }

    return 3;
}

bool GRaceParameters::GetCanBeReversed() const {
    if (mIndex != nullptr) {
        return (mIndex->mFlags & GRaceIndexData::kRaceFlag_CanBeReversed) != 0;
    }

    EnsureLoaded();

    return false;
}

GCharacter *GRaceParameters::GetOpponentChar(unsigned int index) const {
    EnsureLoaded();

    if (GetIsBossRace() && GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career) {
        return GRuntimeInstance::FindObject<GCharacter>(Attrib::StringToLowerCaseKey("character_smart"));
    }

    return GRuntimeInstance::FindObject<GCharacter>(mRaceRecord->Opponents(index));
}

int GRaceParameters::GetNumOpponents() const {
    EnsureLoaded();

    return mRaceRecord->Num_Opponents();
}

void GRaceParameters::GetStartPosition(UMath::Vector3 &pos) const {
    EnsureLoaded();

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int startKey = mRaceRecord->racestart();
    if (startKey != 0) {
        Attrib::Gen::gameplay start(startKey, 0, NULL);

        ExtractPosition(start, result);
    }

    pos = result;
}

void GRaceParameters::GetStartDirection(UMath::Vector3 &dir) const {
    EnsureLoaded();

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int startKey = mRaceRecord->racestart();
    if (startKey != 0) {
        Attrib::Gen::gameplay start(startKey, 0, NULL);

        ExtractDirection(start, result, 0.0f);
    }

    dir = result;
}

bool GRaceParameters::HasFinishLine() const {
    EnsureLoaded();

    return mRaceRecord->racefinish() != 0;
}

void GRaceParameters::GetFinishPosition(UMath::Vector3 &pos) const {
    EnsureLoaded();

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int finishKey = mRaceRecord->racefinish();
    if (finishKey != 0) {
        Attrib::Gen::gameplay finish(finishKey, 0, NULL);

        ExtractPosition(finish, result);
    }

    pos = result;
}

void GRaceParameters::GetFinishDirection(UMath::Vector3 &dir) const {
    EnsureLoaded();

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int finishKey = mRaceRecord->racefinish();
    if (finishKey != 0) {
        Attrib::Gen::gameplay finish(finishKey, 0, NULL);

        ExtractDirection(finish, result, 0.0f);
    }

    dir = result;
}

void GRaceParameters::GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const {
    if (mParentVault != NULL && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault != NULL && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int checkpointKey = mRaceRecord->Checkpoint(index);
    if (checkpointKey != 0) {
        Attrib::Gen::gameplay checkpoint(checkpointKey, 0, NULL);

        ExtractPosition(checkpoint, result);
    }

    pos = result;
}

void GRaceParameters::GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const {
    EnsureLoaded();

    UMath::Vector3 result = UMath::Vector3::kZero;

    unsigned int checkpointKey = mRaceRecord->Checkpoint(index);
    if (checkpointKey != 0) {
        Attrib::Gen::gameplay checkpoint(checkpointKey, 0, NULL);

        ExtractDirection(checkpoint, result, 0.0f);
    }

    dir = result;
}

GMarker *GRaceParameters::GetShortcut(unsigned int index) const {
    EnsureLoaded();

    return GRuntimeInstance::FindObject<GMarker>(mRaceRecord->Shortcuts(index));
}

GMarker *GRaceParameters::GetBarrierExemption(unsigned int index) const {
    EnsureLoaded();

    return GRuntimeInstance::FindObject<GMarker>(mRaceRecord->BarrierExemptions(index));
}

const char *GRaceParameters::GetBarrierName(unsigned int index) const {
    EnsureLoaded();

    const char *name = mRaceRecord->Barriers(index);

    if (name != NULL && *name == '*') {
        name++;
    }

    return name;
}

bool GRaceParameters::GetBarrierIsFlipped(unsigned int index) const {
    EnsureLoaded();

    const char *name = mRaceRecord->Barriers(index);

    if (name != NULL) {
        return *name == '*';
    }

    return false;
}

GRaceCustom::GRaceCustom(const GRaceParameters &other) : GRaceParameters(other.GetCollectionKey(), NULL) {
    mRaceActivity = NULL;
    mNumOpponents = 0;
    mReversed = false;
    mFreedByOwner = false;
    mHeatLevel = -1;

    unsigned int uniqueKey = mRaceRecord->GenerateUniqueKey("GRaceCustom", true);
    // `gameplay::Modify` ya suma LocalAttribCount() dentro del inline, igual
    // que `Instance::Modify`, asi que aqui va el 0 y GCC lo pliega.
    mRaceRecord->Modify(uniqueKey, 0);

    mRaceRecord->SetParent(other.GetGameplayObj()->GetCollection());

    mNumOpponents = mRaceRecord->Num_Opponents();
}

GRaceCustom::~GRaceCustom() {
    if (mRaceActivity != NULL) {
        delete mRaceActivity;
        mRaceActivity = NULL;
    }
}

void GRaceCustom::CreateRaceActivity() {
    if (mReversed) {
        if (GetCanBeReversed()) {
            GCollectionKey startReverse = mRaceRecord->racestartReverse();
            GCollectionKey finishReverse = mRaceRecord->racefinishReverse();
            mRaceRecord->SET_racestart(startReverse);
            mRaceRecord->SET_racefinish(finishReverse);
        } else {
            mReversed = false;
        }
    }

    int existingOpponents = GetNumOpponents();
    bool bossINQuickRace = false;
    if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career && GetIsBossRace()) {
        bossINQuickRace = true;
        existingOpponents = 0;
    }

    if (mNumOpponents != existingOpponents) {
        unsigned int opponentKey[16];
        unsigned int currOpponents = 0;

        if (!bossINQuickRace) {
            currOpponents = mRaceRecord->Num_Opponents();
            for (unsigned int onOpp = 0; onOpp < currOpponents; onOpp++) {
                opponentKey[onOpp] = mRaceRecord->Opponents(onOpp).GetCollectionKey();
            }
        }

        if ((unsigned int)mNumOpponents > currOpponents) {
            if (currOpponents == 0) {
                for (unsigned int onSet = 0; onSet < (unsigned int)mNumOpponents; onSet++) {
                    opponentKey[onSet] = 0x9f3b88c5;
                }
            } else {
                for (unsigned int onCopy = currOpponents; onCopy < (unsigned int)mNumOpponents; onCopy++) {
                    opponentKey[onCopy] = opponentKey[onCopy % currOpponents];
                }
            }
        }

        mRaceRecord->Add(Attrib::Hash::gameplay::Opponents, mNumOpponents);
        Attrib::Attribute attribute = mRaceRecord->Get(Attrib::Hash::gameplay::Opponents);
        for (unsigned int onSet = 0; onSet < (unsigned int)mNumOpponents; onSet++) {
            attribute.Set(onSet, GCollectionKey(opponentKey[onSet]));
        }
    }

    if (mHeatLevel != -1) {
        if (mHeatLevel == 0) {
            SetCopsEnabled(false);
        } else {
            SetForceHeatLevel(mHeatLevel);
        }
    }

    mRaceActivity = new GActivity(mRaceRecord->GetCollection());
    mRaceActivity->AllocateConnectionBuffer(GObjectBlock::CalcNumConnections(mRaceRecord->GetCollection()));
}

GActivity *GRaceCustom::GetRaceActivity() const {
    return mRaceActivity;
}

void GRaceCustom::GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const {
    if (mReversed) {
        unsigned int numCheckpoints = GetNumCheckpoints();
        unsigned int reverseIndex = index + 1;
        index = numCheckpoints - reverseIndex;
    }

    GRaceParameters::GetCheckpointPosition(index, pos);
}

void GRaceCustom::GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const {
    if (mReversed) {
        unsigned int numCheckpoints = GetNumCheckpoints();
        unsigned int reverseIndex = index + 1;
        index = numCheckpoints - reverseIndex;
    }

    UMath::Vector3 checkDir = UMath::Vector3::kZero;

    unsigned int raceCheckSpec = mRaceRecord->Checkpoint(index);
    if (raceCheckSpec != 0) {

        Attrib::Gen::gameplay checkTriggerCollection(raceCheckSpec, 0, NULL);

        ExtractDirection(checkTriggerCollection, checkDir, mReversed ? 180.0f : 0.0f);
    }

    dir = checkDir;
}

void GRaceCustom::SetReversed(bool isReverseDir) {
    mReversed = isReverseDir;
}

template <typename T>
void GRaceCustom::SetAttribute(unsigned int key, const T &value, unsigned int index) {
    Attrib::Attribute attribute = mRaceRecord->Get(key);

    if (!attribute.IsValid()) {
        mRaceRecord->Add(key, 1);
        attribute = mRaceRecord->Get(key);
    }

    T *data = (T *)attribute.GetElementPointer(index);
    if (data != nullptr) {
        *data = value;
    }
}

template void GRaceCustom::SetAttribute<int>(unsigned int, const int &, unsigned int);
template void GRaceCustom::SetAttribute<bool>(unsigned int, const bool &, unsigned int);

void GRaceCustom::SetNumLaps(int numLaps) {
    SetAttribute<int>(Attrib::Hash::gameplay::NumLaps, numLaps, 0);
}

void GRaceCustom::SetTrafficDensity(int density) {
    SetAttribute<int>(Attrib::Hash::gameplay::TrafficLevel, UMath::Clamp(density, 0, 100), 0);
}

void GRaceCustom::SetNumOpponents(int numOpponents) {
    mNumOpponents = numOpponents;
}

void GRaceCustom::SetDifficulty(GRace::Difficulty difficulty) {
    int level;
    switch (difficulty) {
    case GRace::kRaceDifficulty_Easy:
        level = 33;
        break;
    case GRace::kRaceDifficulty_Medium:
        level = 66;
        break;
    case GRace::kRaceDifficulty_Hard:
        level = 100;
        break;
    default:
        level = 0;
        break;
    }
    SetAttribute<int>(Attrib::Hash::gameplay::DifficultyLevel, (int) level, 0);
}

void GRaceCustom::SetCatchUp(bool catchUpEnabled) {
    SetAttribute<bool>(Attrib::Hash::gameplay::CatchUp, catchUpEnabled, 0);
}

void GRaceCustom::SetCopsEnabled(bool copsEnabled) {
    SetAttribute<bool>(Attrib::Hash::gameplay::CopsInRace, copsEnabled, 0);
}

#ifdef EA_BUILD_A124
// Alpha 124 (PS2, 0x2ED580): `jr ra; nop`.
void GRaceCustom::SetCopDensity(GRace::CopDensity copDensity) {}
#endif

void GRaceCustom::SetForceHeatLevel(int level) {
    SetAttribute<int>(Attrib::Hash::gameplay::ForceHeatLevel, level, 0);
}

GRaceBin::GRaceBin(unsigned int collectionKey) : mBinRecord(collectionKey, 0, 0), mChildVault(NULL) {
    bMemSet(&mStats, 0, sizeof(BinStats));

    const char *vaultName = mBinRecord.gameplayvault();
    if (vaultName != NULL) {
        mChildVault = GManager::Get().FindVault(vaultName);
        mChildVault->SetRaceBin();
    }
}

unsigned int GRaceBin::GetCollectionKey() const {
    return mBinRecord.GetCollection();
}

GVault *GRaceBin::GetChildVault() const {
    return mChildVault;
}

int GRaceBin::GetBinNumber() const {
    return mBinRecord.BinIndex();
}

float GRaceBin::GetBaseOpenWorldHeat() const {
    return mBinRecord.BaseOpenWorldHeat();
}

float GRaceBin::GetMaxOpenWorldHeat() const {
    return mBinRecord.MaxOpenWorldHeat();
}

float GRaceBin::GetScaleOpenWorldHeat() const {
    return mBinRecord.ScaleOpenWorldHeat();
}

unsigned int GRaceBin::GetBossRaceCount() const {
    return mBinRecord.Num_BossRaces();
}

unsigned int GRaceBin::GetBossRaceHash(unsigned int index) const {
    unsigned int raceKey = mBinRecord.BossRaces(index);

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromKey(raceKey);

    return parms != nullptr ? parms->GetEventHash() : 0;
}

unsigned int GRaceBin::GetWorldRaceCount() const {
    return mBinRecord.Num_WorldRaces();
}

unsigned int GRaceBin::GetWorldRaceHash(unsigned int index) const {
    unsigned int raceKey = mBinRecord.WorldRaces(index);

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromKey(raceKey);

    return parms != nullptr ? parms->GetEventHash() : 0;
}

unsigned int GRaceBin::GetBaselineUnlockCount() const {
    return mBinRecord.Num_BaselineUnlocks();
}

unsigned int GRaceBin::GetBaselineUnlock(unsigned int index) const {
    return mBinRecord.BaselineUnlocks(index);
}

unsigned int GRaceBin::GetBarrierCount() const {
    return mBinRecord.Num_Barriers();
}

const char *GRaceBin::GetBarrierName(unsigned int index) const {
    const char *name = mBinRecord.Barriers(index);

    if (name != nullptr && name[0] == '*') {
        name++;
    }

    return name;
}

bool GRaceBin::GetBarrierIsFlipped(unsigned int index) const {
    const char *name = mBinRecord.Barriers(index);

    if (name != nullptr) {
        return name[0] == '*';
    }

    return false;
}

void GRaceBin::EnableBarriers() {
    for (unsigned int i = 0; i < GetBarrierCount(); i++) {
        const char *barrierName = GetBarrierName(i);
        EnableBarrierSceneryGroup(barrierName, GetBarrierIsFlipped(i));
    }
    WCollisionAssets::Get().SetExclusionFlags();
    WRoadNetwork::Get().ResolveBarriers();
}

void GRaceBin::DisableBarriers() {
    RedoTopologyAndSceneryGroups();
    WRoadNetwork::Get().ResetBarriers();
    WRoadNetwork::Get().ResetRaceSegments();
}

int GRaceBin::GetRequiredBounty() const {
    return mBinRecord.RequiredBounty();
}

int GRaceBin::GetRequiredChallenges() const {
    return mBinRecord.RequiredChallenges();
}

int GRaceBin::GetRequiredRaceWins() const {
    return mBinRecord.RequiredRacesWon();
}

int GRaceBin::GetCompletedChallenges() const {
    return mStats.mChallengesCompleted;
}

int GRaceBin::GetAwardedRaceWins() const {
    return mStats.mRacesWon;
}

unsigned int GRaceBin::Serialize(unsigned char *dest) {
    bMemCpy(dest, &mStats, sizeof(BinStats));
    return sizeof(BinStats);
}

unsigned int GRaceBin::Deserialize(unsigned char *src) {
    bMemCpy(&mStats, src, sizeof(BinStats));
    return sizeof(BinStats);
}

void GRaceBin::SetCompletedChallenges(int numChallenges) {
    mStats.mChallengesCompleted = numChallenges;
}

void GRaceBin::SetRacesWon(int numRaces) {
    mStats.mRacesWon = numRaces;
}

void GRaceBin::RefreshProgress() {
    int racesWon = 0;

    for (unsigned int i = 0; i < GetWorldRaceCount(); i++) {
        unsigned int raceHash = GetWorldRaceHash(i);
        if (GRaceDatabase::Get().CheckRaceScoreFlags(raceHash, GRaceDatabase::kCompleted_ContextCareer)) {
            racesWon++;
        }
    }

    int challengesCompleted = 0;

    for (GMilestone *milestone = GManager::Get().GetFirstMilestone(false, GetBinNumber()); milestone != NULL;
         milestone = GManager::Get().GetNextMilestone(milestone, false, GetBinNumber())) {
        if (milestone->GetIsAwarded()) {
            challengesCompleted++;
        }
    }

    for (GSpeedTrap *speedTrap = GManager::Get().GetFirstSpeedTrap(false, GetBinNumber()); speedTrap != NULL;
         speedTrap = GManager::Get().GetNextSpeedTrap(speedTrap, false, GetBinNumber())) {
        if (speedTrap->IsFlagSet(GSpeedTrap::kFlag_Completed)) {
            challengesCompleted++;
        }
    }

    SetRacesWon(racesWon);
    SetCompletedChallenges(challengesCompleted);
}

void GRaceDatabase::SimulateDDayComplete() {
}
