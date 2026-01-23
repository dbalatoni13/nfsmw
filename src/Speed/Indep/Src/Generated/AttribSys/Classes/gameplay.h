#ifndef ATTRIBSYS_CLASSES_GAMEPLAY_H
#define ATTRIBSYS_CLASSES_GAMEPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Gameplay/GReflected.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO move maybe? unfortunately I can't find where GetGameplayType is inlined from
enum GameplayObjType {
    kGameplayObjType_Invalid = -1,
    kGameplayObjType_Activity = 0,
    kGameplayObjType_Character = 1,
    kGameplayObjType_Handler = 2,
    kGameplayObjType_Marker = 3,
    kGameplayObjType_State = 4,
    kGameplayObjType_Trigger = 5,
    kGameplayObjType_Count = 6,
};

namespace Attrib {
namespace Gen {

struct gameplay : Instance {
    struct _LayoutStruct {
        char CollectionName[4];  // offset 0x0, size 0x4
        unsigned int message_id; // offset 0x4, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "gameplay");
    }

    gameplay(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    gameplay(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~gameplay() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x5cea9d46;
    }

    const float &GoalAddPrevBest(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x006ec903, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &TargetBronze(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x00df8eb4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &PursuitLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x0261ae99, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &PostRaceScreenTexture(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x038a3b53, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &MiniMapItem(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x051e90ca, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &CashRewards(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x0550fbc2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_CashRewards() const {
        return this->Get(0x0550fbc2).GetLength();
    }

    const EA::Reflection::Text &RewardMarkerType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x06a077d5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &MilestoneBiggerIsBetter(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x0896d043, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &InitialSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x0a91596d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &FilterModePassAll(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x0d038cfa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &InternalRaceIndex(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x0d4c1055, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    unsigned int Num_InternalRaceIndex() const {
        return this->Get(0x0d4c1055).GetLength();
    }

    const EA::Reflection::Text &UpgradeType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x0e0113fe, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &OutroNISMarker(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x0e265c88, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &SharedCheckpoints(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x0e34a1f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &ScriptedCopsInRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x0e47fe63, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &NumLaps(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x0ebdc165, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &TargetActivities(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x0f37d221, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_TargetActivities() const {
        return this->Get(0x0f37d221).GetLength();
    }

    const EA::Reflection::Text &EventIconType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x0f6bcde1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &disengagetrigger(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x106285c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &CatchUp(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x10db04e6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &RoadList(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x13b11b40, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    unsigned int Num_RoadList() const {
        return this->Get(0x13b11b40).GetLength();
    }

    const float &ShortcutMaxChance(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x16faba11, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &KnockoutsPerLap(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x181462da, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &ScaleOpenWorldHeat(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x1823b89e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &target(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x1a7d2859, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &OpenWorldSpeedTrap(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x1bb16f14, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &ChallengeSeriesRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x1c650104, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &BossReputation(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x1d33241a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &DelayTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x20259346, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &RespawnMarker(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x2241f4cd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &transitionlist(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x25621dc5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_transitionlist() const {
        return this->Get(0x25621dc5).GetLength();
    }

    const int &OvertimePenaltyPerSec(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x26fd42b0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &TargetActivity(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x277566f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &AutoActivateGPS(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x29b9c312, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &CheckpointsVisible(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x2ad67092, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &PursuitRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x2b1f54f6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &ResetTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x2c44ff10, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &Checkpoint(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x34aae3fc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_Checkpoint() const {
        return this->Get(0x34aae3fc).GetLength();
    }

    const int &IntroCameraTrack(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x36bbeee9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &CopsInRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x3918e889, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &FlareSpacing(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x394abbc6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &AvailableOnline(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x39509746, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &Radius(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x39bf8002, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &InitialPlayerSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x3a0e4b19, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &SkillLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x3b798aa2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &GoalHard(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x3b9bbfc2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &MedalBonusBronze(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x3bb31211, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &UpgradePartID(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x3c2fdaab, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &Name(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x3e225ec1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &DoCountdown(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x3e33da0f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &Template(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x3e9156ca, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &IconModelFloatHeight(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x4037d3c5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const UMath::Vector2 &layoutpos(unsigned int index) const {
        const UMath::Vector2 *resultptr = reinterpret_cast<const UMath::Vector2 *>(this->GetAttributePointer(0x4075ec46, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
        return *resultptr;
    }

    const bool &NoPostRaceScreen(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x40f9929f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &PresetRide(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x416a8409, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &IsEpicPursuitRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x4393f69b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &CatchUpIntegral(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x4545ab74, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &UseWorldHeat(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x45f2ad6c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &BustedLives(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x462f2e36, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &Reputation(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x477ec5aa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &parentstate(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x4acc6d63, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &NISShell(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x4c17fe41, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &ThreshholdValue(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x4e90219d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &ShortcutMinChance(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x4efb950a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &RewardsForWinner(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x50104d90, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_RewardsForWinner() const {
        return this->Get(0x50104d90).GetLength();
    }

    const float &CatchUpDerivative(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x515aa4e4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &TargetSilver(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x51ce16b7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &CameraModelMarker(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x52dc742c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const int &IntroMessageID(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x5468366d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &ChanceOfRain(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x547486ae, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &OutroNIS(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x54932966, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const Attrib::Blob &FilterBlocks(unsigned int index) const {
        const Attrib::Blob *resultptr = reinterpret_cast<const Attrib::Blob *>(this->GetAttributePointer(0x56e1436d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Blob *>(DefaultDataArea(sizeof(Attrib::Blob)));
        }
        return *resultptr;
    }

    unsigned int Num_FilterBlocks() const {
        return this->Get(0x56e1436d).GetLength();
    }

    const float &Width(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x5816c1fc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &Opponents(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x5839fa1a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_Opponents() const {
        return this->Get(0x5839fa1a).GetLength();
    }

    const GCollectionKey &RaceTriggers(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x58dc14c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_RaceTriggers() const {
        return this->Get(0x58dc14c0).GetLength();
    }

    const EA::Reflection::Text &QuickRaceNIS(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x5987fb25, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &Rotation(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x5a6a57c6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &ForceTrafficDensity(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x5e161bba, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &RankPlayersByPoints(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x5ec1880f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &ParticleEffect(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x5ef34802, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &SpeedTrapTrigger(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x5f95c3a0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &MasterCheckpoint(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x609febe8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &FinishCamera(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x62dfc259, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &TrafficPattern(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x6319b692, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &CollectorsEditionRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x637584fe, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &PostRaceActivity(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x64273c71, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &ZoneList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x64893da8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_ZoneList() const {
        return this->Get(0x64893da8).GetLength();
    }

    const float &IconModelSpinRate(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x697332e8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &SMSCellChallenge(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x6a4cd2d4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &IsLoopingRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x6a9a6f5b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &Directional(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x6b37e124, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &ResetsPlayer(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x6ccd5819, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &BinIndex(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x6ce23062, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &SpeedTrapsRequired(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x6d7e73c9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_SpeedTrapsRequired() const {
        return this->Get(0x6d7e73c9).GetLength();
    }

    const UMath::Vector3 &Dimensions(unsigned int index) const {
        const UMath::Vector3 *resultptr = reinterpret_cast<const UMath::Vector3 *>(this->GetAttributePointer(0x6d9e21ad, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector3 *>(DefaultDataArea(sizeof(UMath::Vector3)));
        }
        return *resultptr;
    }

    const int &RequiredChallenges(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x6dd4b98b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &MilestoneName(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x704f72e8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const int &OutroCameraTrack(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x7054ff5b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &scriptname(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x7148ae82, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &TargetGold(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x728e43ff, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &ProgressionLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x740e9b4a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &TimeLimit(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x7585f041, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &MedalBonusSilver(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x767b00a9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &KnockoutTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x777ece27, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &TrafficCharacter(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x797d9654, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &IsBoss(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x79c5d68d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &Shortcuts(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x7b6d296e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_Shortcuts() const {
        return this->Get(0x7b6d296e).GetLength();
    }

    const float &RaceLength(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x7c11c52e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &racefinishReverse(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x7c7cf20f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &CatchUpSkill(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x8069b5a9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &SpeedTrapList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x822179d1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_SpeedTrapList() const {
        return this->Get(0x822179d1).GetLength();
    }

    const float &StartTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x839602ab, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &GoalEasy(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x8445af47, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &handler_owner(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x857fe432, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &AutoStart(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x883c65e3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &DifficultyLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x88a7e3be, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &DDayRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x8cb01abf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &Bounty(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x8e1904c7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &BaseOpenWorldHeat(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x8f186ac4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &TargetMarker(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x8fc356fb, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const int &NumRacesRequired(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x90a22a3f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &Children(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x916e0e78, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_Children() const {
        return this->Get(0x916e0e78).GetLength();
    }

    const GCollectionKey &stateref(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x918c796e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &gameplayvault(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0x93fd9fda, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &ForcePreload(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x9652af0f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const Attrib::Blob &bytecode(unsigned int index) const {
        const Attrib::Blob *resultptr = reinterpret_cast<const Attrib::Blob *>(this->GetAttributePointer(0x9a4a020a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Blob *>(DefaultDataArea(sizeof(Attrib::Blob)));
        }
        return *resultptr;
    }

    const GCollectionKey &CannedPath(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x9c19e56f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_CannedPath() const {
        return this->Get(0x9c19e56f).GetLength();
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const float &TOD(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x9dff3c3d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &RankPlayersByDistance(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x9e7a18ce, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &CatchUpOverride(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x9eb17c1e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const UMath::Vector3 &Position(unsigned int index) const {
        const UMath::Vector3 *resultptr = reinterpret_cast<const UMath::Vector3 *>(this->GetAttributePointer(0x9f743a0e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector3 *>(DefaultDataArea(sizeof(UMath::Vector3)));
        }
        return *resultptr;
    }

    const GCollectionKey &RaceList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0x9f914008, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_RaceList() const {
        return this->Get(0x9f914008).GetLength();
    }

    const GCollectionKey &templateref(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa0697302, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const float &MaxPursuitRep(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xa07ae814, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &AllowInvisibleSpawn(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xa1009a23, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &CatchUpSpread(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xa18a07ba, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &NeverInQuickRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xa4e6fcfd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &sender(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa590a98b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &ExcludedCharacters(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa5c5d25b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_ExcludedCharacters() const {
        return this->Get(0xa5c5d25b).GetLength();
    }

    const GCollectionKey &Boss(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa5f39dc7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &IconModelName(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xa62cb4f0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &EventID(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xa78403ec, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &DebugJumpPoint(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa7e9e456, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &WorldRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xa7ef40ef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_WorldRaces() const {
        return this->Get(0xa7ef40ef).GetLength();
    }

    const bool &FreeRoamOnly(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xaa0135e9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &TokenValue(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xaa10914c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &CashReward(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xab0179f4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &racefinish(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xb0a24adc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const float &MinimumAIPerformance() const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb1ece070, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &FireOnExit(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xb2ac32c7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &AvailableQR(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xb39ed8c3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &IconModelBounceRate(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb4985085, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &JumpRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xb671abb6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_JumpRaces() const {
        return this->Get(0xb671abb6).GetLength();
    }

    const EA::Reflection::Text &OutroMovie(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xb70268c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &RollingStart(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xb809d19c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &BaselineUnlocks(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xbaf89280, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_BaselineUnlocks() const {
        return this->Get(0xbaf89280).GetLength();
    }

    const int &SMSRivalChallenge(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xbb30c804, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &MilestoneChallenge(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xbcd98737, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &RacerName(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xbeab64c5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &PlayerCarType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xc0eeb909, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &EntryActivity(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xc27dfda8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const int &OutroMessageID(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xc36e3532, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &ThreshholdSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xc3710777, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &UpgradePartName(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xc385f75d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &QuickRaceUnlocked(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xc4db4e71, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &RingTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xc516e9c2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &distance(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xc5857615, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &TrafficLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xc64bc341, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &CellChallengeRace(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xc686cd34, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &GateActivity(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xc795b8d4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &Region(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xcb01e454, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &SpeedTrapCamera(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xcbd7adf9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const bool &IconModelScale(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xcd41cd40, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &OneShot(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xce4261ac, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &UpgradeLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xd267facc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &RequiredBounty(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xd3657d92, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &nitrouslevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xd4797aa8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &BossRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xd5a174aa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_BossRaces() const {
        return this->Get(0xd5a174aa).GetLength();
    }

    const EA::Reflection::Text &AutoSpawnTriggerType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xd5c7e9c3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &IconModelBounceAmp(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xd5f4eda2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &RequiredRacesWon(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xd617fedc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &CopSpawnType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xd686d61e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &CashValue(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xd8baa07b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &LocalizationTag(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xdb89ab5c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &CopDensity(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xdbc08d32, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &RestartActivity(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xdc44bd08, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &SpawnPoint(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xddf411f5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &IntroNIS(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xdec18d3e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &StartPercent(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe0d01505, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &MaxCarRep(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe10fb7a3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &Barriers(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xe244f26b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    unsigned int Num_Barriers() const {
        return this->Get(0xe244f26b).GetLength();
    }

    const int &EntryCellCallID(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xe2d26232, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &ForceHeatLevel(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xe4211f4f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const GCollectionKey &racestart(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xe43b2ccc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const bool &Persistent(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xe4542e9b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &actionscript(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xe62083d0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const float &MaxOpenWorldHeat(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe8c24416, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &InitiallyUnlocked(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xea855eaf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &DoPhotofinish(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xede6017e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &engagetrigger(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xf05931ab, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &CopSpawnPoints(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xf124b151, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_CopSpawnPoints() const {
        return this->Get(0xf124b151).GetLength();
    }

    const bool &IsMarkerRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xf2fe50d7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const GCollectionKey &BarrierExemptions(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xf380286b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_BarrierExemptions() const {
        return this->Get(0xf380286b).GetLength();
    }

    const EA::Reflection::Text &ZoneType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xf3ea3201, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const int &TimeBonus(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xf52cc30e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &IntroMovie(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xf572ede8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &MaxHeatLevel(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xf5a03629, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &MedalBonusGold(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xf5e43987, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const EA::Reflection::Text &CarType(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xf833c06f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const float &RivalBestTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xf9120d73, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &PlayerCarPerformance(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xfb42c0b9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const GCollectionKey &ForceStartPosition(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xfb84be75, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &UnlockRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xfc8995c8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_UnlockRaces() const {
        return this->Get(0xfc8995c8).GetLength();
    }

    const EA::Reflection::Text &CarTypeLowMem(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(this->GetAttributePointer(0xfd3cf790, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }

    const GCollectionKey &racestartReverse(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xfd945479, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    const GCollectionKey &RandomSpawnTriggers(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(this->GetAttributePointer(0xfdfe1c3e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }

    unsigned int Num_RandomSpawnTriggers() const {
        return this->Get(0xfdfe1c3e).GetLength();
    }

    const bool &BossRace(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xff5ee5d6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &ReputationRequired(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xffd69c94, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const unsigned int &message_id() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->message_id;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
