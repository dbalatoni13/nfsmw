#ifndef ATTRIBSYS_CLASSES_GAMEPLAY_H
#define ATTRIBSYS_CLASSES_GAMEPLAY_H


#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct gameplay : Instance {
struct _LayoutStruct {
const char *CollectionName; // offset 0x0, size 0x4
unsigned int message_id; // offset 0x4, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "gameplay");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "gameplay");
}

gameplay(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

gameplay(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

gameplay(const gameplay &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

gameplay(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~gameplay() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

void Change(const RefSpec &refspec) {
    Instance::Change(refspec);
}

static Key ClassKey() {
    return 0x5cea9d46;
}

const float &GoalAddPrevBest() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x006ec903, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &TargetBronze() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x00df8eb4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &PursuitLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x0261ae99, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &PostRaceScreenTexture() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x038a3b53, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &MiniMapItem() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x051e90ca, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &CashRewards(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x0550fbc2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_CashRewards() const {
            return Get(0x0550fbc2).GetLength();
        }

const EA::Reflection::Text &RewardMarkerType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x06a077d5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &MilestoneBiggerIsBetter() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x0896d043, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &InitialSpeed() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0a91596d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &FilterModePassAll() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x0d038cfa, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &InternalRaceIndex(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0d4c1055, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
unsigned int Num_InternalRaceIndex() const {
            return Get(0x0d4c1055).GetLength();
        }

const EA::Reflection::Text &UpgradeType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x0e0113fe, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &OutroNISMarker() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x0e265c88, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &SharedCheckpoints() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x0e34a1f3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &ScriptedCopsInRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x0e47fe63, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &NumLaps() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x0ebdc165, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &TargetActivities(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x0f37d221, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_TargetActivities() const {
            return Get(0x0f37d221).GetLength();
        }

const EA::Reflection::Text &EventIconType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x0f6bcde1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &disengagetrigger() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x106285c0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &CatchUp() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x10db04e6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &RoadList(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x13b11b40, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
unsigned int Num_RoadList() const {
            return Get(0x13b11b40).GetLength();
        }

const float &ShortcutMaxChance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x16faba11, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &KnockoutsPerLap() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x181462da, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &ScaleOpenWorldHeat() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x1823b89e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &target() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x1a7d2859, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &OpenWorldSpeedTrap() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x1bb16f14, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &ChallengeSeriesRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x1c650104, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &BossReputation() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x1d33241a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &DelayTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x20259346, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &RespawnMarker() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x2241f4cd, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &transitionlist(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x25621dc5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_transitionlist() const {
            return Get(0x25621dc5).GetLength();
        }

const int &OvertimePenaltyPerSec() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x26fd42b0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &TargetActivity() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x277566f3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &AutoActivateGPS() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x29b9c312, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &CheckpointsVisible() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x2ad67092, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &PursuitRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x2b1f54f6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &ResetTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x2c44ff10, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &Checkpoint(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x34aae3fc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_Checkpoint() const {
            return Get(0x34aae3fc).GetLength();
        }

const int &IntroCameraTrack() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x36bbeee9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &CopsInRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x3918e889, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &FlareSpacing() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x394abbc6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &AvailableOnline() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x39509746, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &Radius() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x39bf8002, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &InitialPlayerSpeed() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x3a0e4b19, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &SkillLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x3b798aa2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &GoalHard() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x3b9bbfc2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &MedalBonusBronze() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x3bb31211, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &UpgradePartID() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x3c2fdaab, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &Name() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x3e225ec1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &DoCountdown() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x3e33da0f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &Template() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x3e9156ca, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &IconModelFloatHeight() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4037d3c5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const UMath::Vector2 &layoutpos() const {
        const UMath::Vector2 *resultptr = reinterpret_cast<const UMath::Vector2 *>(GetAttributePointer(0x4075ec46, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
        return *resultptr;
    }
        
const bool &NoPostRaceScreen() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x40f9929f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &PresetRide() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x416a8409, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &IsEpicPursuitRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x4393f69b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &CatchUpIntegral() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4545ab74, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &UseWorldHeat() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x45f2ad6c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &BustedLives() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x462f2e36, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &Reputation() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x477ec5aa, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &parentstate() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x4acc6d63, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &NISShell() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x4c17fe41, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &ThreshholdValue() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4e90219d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &ShortcutMinChance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4efb950a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &RewardsForWinner(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x50104d90, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_RewardsForWinner() const {
            return Get(0x50104d90).GetLength();
        }

const float &CatchUpDerivative() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x515aa4e4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &TargetSilver() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x51ce16b7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &CameraModelMarker() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x52dc742c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const int &IntroMessageID() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x5468366d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &ChanceOfRain() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x547486ae, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &OutroNIS() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x54932966, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const Attrib::Blob &FilterBlocks(unsigned int index) const {
        const Attrib::Blob *resultptr = reinterpret_cast<const Attrib::Blob *>(GetAttributePointer(0x56e1436d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Blob *>(DefaultDataArea(sizeof(Attrib::Blob)));
        }
        return *resultptr;
    }
        
unsigned int Num_FilterBlocks() const {
            return Get(0x56e1436d).GetLength();
        }

const float &Width() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x5816c1fc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &Opponents(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x5839fa1a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_Opponents() const {
            return Get(0x5839fa1a).GetLength();
        }

const GCollectionKey &RaceTriggers(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x58dc14c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_RaceTriggers() const {
            return Get(0x58dc14c0).GetLength();
        }

const EA::Reflection::Text &QuickRaceNIS() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x5987fb25, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &Rotation() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x5a6a57c6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &ForceTrafficDensity() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x5e161bba, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &RankPlayersByPoints() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x5ec1880f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &ParticleEffect() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x5ef34802, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &SpeedTrapTrigger() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x5f95c3a0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &MasterCheckpoint() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x609febe8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &FinishCamera() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x62dfc259, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &TrafficPattern() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x6319b692, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &CollectorsEditionRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x637584fe, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &PostRaceActivity() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x64273c71, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &ZoneList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x64893da8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_ZoneList() const {
            return Get(0x64893da8).GetLength();
        }

const float &IconModelSpinRate() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x697332e8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &SMSCellChallenge() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x6a4cd2d4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &IsLoopingRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x6a9a6f5b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &Directional() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x6b37e124, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &ResetsPlayer() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x6ccd5819, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &BinIndex() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x6ce23062, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &SpeedTrapsRequired(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x6d7e73c9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_SpeedTrapsRequired() const {
            return Get(0x6d7e73c9).GetLength();
        }

const UMath::Vector3 &Dimensions() const {
        const UMath::Vector3 *resultptr = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x6d9e21ad, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector3 *>(DefaultDataArea(sizeof(UMath::Vector3)));
        }
        return *resultptr;
    }
        
const int &RequiredChallenges() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x6dd4b98b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &MilestoneName() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x704f72e8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const int &OutroCameraTrack() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x7054ff5b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &scriptname() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x7148ae82, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &TargetGold() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x728e43ff, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &ProgressionLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x740e9b4a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &TimeLimit() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x7585f041, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &MedalBonusSilver() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x767b00a9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &KnockoutTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x777ece27, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &TrafficCharacter() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x797d9654, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &IsBoss() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x79c5d68d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &Shortcuts(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x7b6d296e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_Shortcuts() const {
            return Get(0x7b6d296e).GetLength();
        }

const float &RaceLength() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x7c11c52e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &racefinishReverse() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x7c7cf20f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &CatchUpSkill() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x8069b5a9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &SpeedTrapList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x822179d1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_SpeedTrapList() const {
            return Get(0x822179d1).GetLength();
        }

const float &StartTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x839602ab, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &GoalEasy() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x8445af47, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &handler_owner() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x857fe432, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &AutoStart() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x883c65e3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &DifficultyLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x88a7e3be, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &DDayRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x8cb01abf, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &Bounty() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x8e1904c7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &BaseOpenWorldHeat() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x8f186ac4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &TargetMarker() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x8fc356fb, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const int &NumRacesRequired() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x90a22a3f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &Children(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x916e0e78, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_Children() const {
            return Get(0x916e0e78).GetLength();
        }

const GCollectionKey &stateref() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x918c796e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &gameplayvault() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0x93fd9fda, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &ForcePreload() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x9652af0f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const Attrib::Blob &bytecode() const {
        const Attrib::Blob *resultptr = reinterpret_cast<const Attrib::Blob *>(GetAttributePointer(0x9a4a020a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Blob *>(DefaultDataArea(sizeof(Attrib::Blob)));
        }
        return *resultptr;
    }
        
const GCollectionKey &CannedPath(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x9c19e56f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_CannedPath() const {
            return Get(0x9c19e56f).GetLength();
        }

const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CollectionName;
}

const float &TOD() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x9dff3c3d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &RankPlayersByDistance() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x9e7a18ce, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &CatchUpOverride() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x9eb17c1e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const UMath::Vector3 &Position() const {
        const UMath::Vector3 *resultptr = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x9f743a0e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector3 *>(DefaultDataArea(sizeof(UMath::Vector3)));
        }
        return *resultptr;
    }
        
const GCollectionKey &RaceList(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0x9f914008, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_RaceList() const {
            return Get(0x9f914008).GetLength();
        }

const GCollectionKey &templateref() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa0697302, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const float &MaxPursuitRep() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xa07ae814, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &AllowInvisibleSpawn() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xa1009a23, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &CatchUpSpread() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xa18a07ba, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &NeverInQuickRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xa4e6fcfd, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &sender() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa590a98b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &ExcludedCharacters(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa5c5d25b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_ExcludedCharacters() const {
            return Get(0xa5c5d25b).GetLength();
        }

const GCollectionKey &Boss() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa5f39dc7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &IconModelName() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xa62cb4f0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &EventID() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xa78403ec, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &DebugJumpPoint() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa7e9e456, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &WorldRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xa7ef40ef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_WorldRaces() const {
            return Get(0xa7ef40ef).GetLength();
        }

const bool &FreeRoamOnly() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xaa0135e9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &TokenValue() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xaa10914c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &CashReward() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xab0179f4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &racefinish() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xb0a24adc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const float &MinimumAIPerformance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xb1ece070, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &FireOnExit() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xb2ac32c7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &AvailableQR() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xb39ed8c3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &IconModelBounceRate() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xb4985085, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &JumpRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xb671abb6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_JumpRaces() const {
            return Get(0xb671abb6).GetLength();
        }

const EA::Reflection::Text &OutroMovie() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xb70268c0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &RollingStart() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xb809d19c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &BaselineUnlocks(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xbaf89280, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_BaselineUnlocks() const {
            return Get(0xbaf89280).GetLength();
        }

const int &SMSRivalChallenge() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xbb30c804, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &MilestoneChallenge() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xbcd98737, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &RacerName() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xbeab64c5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &PlayerCarType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xc0eeb909, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &EntryActivity() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xc27dfda8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const int &OutroMessageID() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xc36e3532, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &ThreshholdSpeed() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xc3710777, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &UpgradePartName() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xc385f75d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &QuickRaceUnlocked() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xc4db4e71, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &RingTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xc516e9c2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &distance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xc5857615, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &TrafficLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xc64bc341, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &CellChallengeRace() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xc686cd34, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &GateActivity() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xc795b8d4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &Region() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xcb01e454, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &SpeedTrapCamera() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xcbd7adf9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const bool &IconModelScale() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xcd41cd40, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &OneShot() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xce4261ac, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &UpgradeLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xd267facc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &RequiredBounty() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xd3657d92, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &nitrouslevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xd4797aa8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &BossRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xd5a174aa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_BossRaces() const {
            return Get(0xd5a174aa).GetLength();
        }

const EA::Reflection::Text &AutoSpawnTriggerType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xd5c7e9c3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &IconModelBounceAmp() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xd5f4eda2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &RequiredRacesWon() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xd617fedc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &CopSpawnType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xd686d61e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &CashValue() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xd8baa07b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &LocalizationTag() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xdb89ab5c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &CopDensity() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xdbc08d32, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &RestartActivity() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xdc44bd08, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &SpawnPoint() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xddf411f5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &IntroNIS() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xdec18d3e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &StartPercent() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe0d01505, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &MaxCarRep() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe10fb7a3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &Barriers(unsigned int index) const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xe244f26b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
unsigned int Num_Barriers() const {
            return Get(0xe244f26b).GetLength();
        }

const int &EntryCellCallID() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xe2d26232, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &ForceHeatLevel() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xe4211f4f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const GCollectionKey &racestart() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xe43b2ccc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const bool &Persistent() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xe4542e9b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &actionscript() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xe62083d0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const float &MaxOpenWorldHeat() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe8c24416, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &InitiallyUnlocked() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xea855eaf, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &DoPhotofinish() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xede6017e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &engagetrigger() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xf05931ab, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &CopSpawnPoints(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xf124b151, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_CopSpawnPoints() const {
            return Get(0xf124b151).GetLength();
        }

const bool &IsMarkerRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xf2fe50d7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const GCollectionKey &BarrierExemptions(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xf380286b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_BarrierExemptions() const {
            return Get(0xf380286b).GetLength();
        }

const EA::Reflection::Text &ZoneType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xf3ea3201, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const int &TimeBonus() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xf52cc30e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &IntroMovie() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xf572ede8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &MaxHeatLevel() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xf5a03629, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &MedalBonusGold() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xf5e43987, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EA::Reflection::Text &CarType() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xf833c06f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const float &RivalBestTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xf9120d73, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &PlayerCarPerformance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xfb42c0b9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const GCollectionKey &ForceStartPosition() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xfb84be75, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &UnlockRaces(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xfc8995c8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_UnlockRaces() const {
            return Get(0xfc8995c8).GetLength();
        }

const EA::Reflection::Text &CarTypeLowMem() const {
        const EA::Reflection::Text *resultptr = reinterpret_cast<const EA::Reflection::Text *>(GetAttributePointer(0xfd3cf790, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EA::Reflection::Text *>(DefaultDataArea(sizeof(EA::Reflection::Text)));
        }
        return *resultptr;
    }
        
const GCollectionKey &racestartReverse() const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xfd945479, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
const GCollectionKey &RandomSpawnTriggers(unsigned int index) const {
        const GCollectionKey *resultptr = reinterpret_cast<const GCollectionKey *>(GetAttributePointer(0xfdfe1c3e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const GCollectionKey *>(DefaultDataArea(sizeof(GCollectionKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_RandomSpawnTriggers() const {
            return Get(0xfdfe1c3e).GetLength();
        }

const bool &BossRace() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xff5ee5d6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &ReputationRequired() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xffd69c94, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const unsigned int &message_id() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->message_id;
}

};

} // namespace Gen
} // namespace Attrib

#endif
