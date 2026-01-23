#ifndef ATTRIBSYS_CLASSES_PURSUITLEVELS_H
#define ATTRIBSYS_CLASSES_PURSUITLEVELS_H


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

struct pursuitlevels : Instance {
struct _LayoutStruct {
Private _Array_cops; // offset 0x0, size 0x8
CopCountRecord cops[3]; // offset 0x8, size 0x48
float heliLOSdistance; // offset 0x50, size 0x4
float roadblockhelichance; // offset 0x54, size 0x4
float SirenMaxYelpTime; // offset 0x58, size 0x4
float roadblockprobability; // offset 0x5c, size 0x4
float MilestoneCompleteHeatAdjust; // offset 0x60, size 0x4
float SirenMaxScreamTime; // offset 0x64, size 0x4
float SearchModeRoadblockChance; // offset 0x68, size 0x4
float HeliFuelTime; // offset 0x6c, size 0x4
int FullEngagementCopCount; // offset 0x70, size 0x4
float TimeBetweenHeliActive; // offset 0x74, size 0x4
float TimePerHeatLevel; // offset 0x78, size 0x4
float SirenWailPeriod; // offset 0x7c, size 0x4
float TimeBetweenCopSpawn; // offset 0x80, size 0x4
float EventWinHeatAdjust; // offset 0x84, size 0x4
int RepPointsPerMinute; // offset 0x88, size 0x4
float SearchModeCityMPH; // offset 0x8c, size 0x4
float evadetimeout; // offset 0x90, size 0x4
float SearchModeHwyMPH; // offset 0x94, size 0x4
float FullEngagementRadius; // offset 0x98, size 0x4
int NumCopsToTriggerBackup; // offset 0x9c, size 0x4
float BackupCallTimer; // offset 0xa0, size 0x4
float SirenInitVariation; // offset 0xa4, size 0x4
float EvadeSuccessHeatAdjust; // offset 0xa8, size 0x4
float SirenInitMinPeriod; // offset 0xac, size 0x4
float frontLOSdistance; // offset 0xb0, size 0x4
float TimeToHideInZone; // offset 0xb4, size 0x4
float rearLOSdistance; // offset 0xb8, size 0x4
float SirenScreamPeriod; // offset 0xbc, size 0x4
float SearchModeRoadblockRadius; // offset 0xc0, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "pursuitlevels");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "pursuitlevels");
}

pursuitlevels(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

pursuitlevels(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

pursuitlevels(const pursuitlevels &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

pursuitlevels(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~pursuitlevels() {}

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
    return 0x551e22b3;
}

const float &TimeInactiveFor911() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x06cb70d5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &MaxCopsCollapsing() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x0c77d463, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &Lifetime911() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0e823327, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &SpeedReactionTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0f575b64, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &CollapseInnerRadius() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x1e0af662, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &NumPatrolCars() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x24f7a1bc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &SearchModeHeliSpawnChance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x3f11fbfc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &CollapseAggression() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x594e1492, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &roadblockspikechance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x5a318af6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const CopFormationRecord &CopFormations(unsigned int index) const {
        const CopFormationRecord *resultptr = reinterpret_cast<const CopFormationRecord *>(GetAttributePointer(0x5c2a7972, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const CopFormationRecord *>(DefaultDataArea(sizeof(CopFormationRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_CopFormations() const {
            return Get(0x5c2a7972).GetLength();
        }

const float &RollingBlockDuration() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x5c9f5f55, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &BoxinTightness() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x67a15750, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const char &NumCiviHitsFor911() const {
        const char *resultptr = reinterpret_cast<const char *>(GetAttributePointer(0x6e590f57, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const char *>(DefaultDataArea(sizeof(char)));
        }
        return *resultptr;
    }
        
const float &StaggerFormationTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x7648c884, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &BustSpeed() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x769e8d9e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &HiddenZoneTimeMultiplier() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x7fcee250, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &ScaleEscalationPerBucket(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x80deb840, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
unsigned int Num_ScaleEscalationPerBucket() const {
            return Get(0x80deb840).GetLength();
        }

const float &BoxinDuration() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x858b1097, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &MeterDeadZoneBustedDistance() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x8ce3219f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const unsigned int &formations(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x92f3d64e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
unsigned int Num_formations() const {
            return Get(0x92f3d64e).GetLength();
        }

const float &CollapseOuterRadius() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x947542f2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &TimeBetweenFirstFourSpawn() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x9bf0f433, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &CTSFor911() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xa00de933, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const CopCountRecord &cops(unsigned int index) const {
            const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(GetLayoutPointer());
            if (index < lp->_Array_cops.GetLength()) {
            return lp->cops[index];
        } else {
            return *reinterpret_cast<const CopCountRecord *>(DefaultDataArea(sizeof(CopCountRecord)));
        }
        }

        unsigned int Num_cops() const {
            return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_Array_cops.GetLength();
        }
        
        const float &DestroyCopBonusTime() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xbef78612, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &RollingBlockTightness() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xc146fc03, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &CollapseSpeed() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xdb66950c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &MeterDeadZoneEvadeDist() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xeda0e29c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &heliLOSdistance() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->heliLOSdistance;
}

const float &roadblockhelichance() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->roadblockhelichance;
}

const float &SirenMaxYelpTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenMaxYelpTime;
}

const float &roadblockprobability() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->roadblockprobability;
}

const float &MilestoneCompleteHeatAdjust() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MilestoneCompleteHeatAdjust;
}

const float &SirenMaxScreamTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenMaxScreamTime;
}

const float &SearchModeRoadblockChance() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SearchModeRoadblockChance;
}

const float &HeliFuelTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->HeliFuelTime;
}

const int &FullEngagementCopCount() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FullEngagementCopCount;
}

const float &TimeBetweenHeliActive() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimeBetweenHeliActive;
}

const float &TimePerHeatLevel() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimePerHeatLevel;
}

const float &SirenWailPeriod() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenWailPeriod;
}

const float &TimeBetweenCopSpawn() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimeBetweenCopSpawn;
}

const float &EventWinHeatAdjust() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->EventWinHeatAdjust;
}

const int &RepPointsPerMinute() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->RepPointsPerMinute;
}

const float &SearchModeCityMPH() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SearchModeCityMPH;
}

const float &evadetimeout() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->evadetimeout;
}

const float &SearchModeHwyMPH() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SearchModeHwyMPH;
}

const float &FullEngagementRadius() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FullEngagementRadius;
}

const int &NumCopsToTriggerBackup() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->NumCopsToTriggerBackup;
}

const float &BackupCallTimer() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BackupCallTimer;
}

const float &SirenInitVariation() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenInitVariation;
}

const float &EvadeSuccessHeatAdjust() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->EvadeSuccessHeatAdjust;
}

const float &SirenInitMinPeriod() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenInitMinPeriod;
}

const float &frontLOSdistance() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->frontLOSdistance;
}

const float &TimeToHideInZone() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimeToHideInZone;
}

const float &rearLOSdistance() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->rearLOSdistance;
}

const float &SirenScreamPeriod() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SirenScreamPeriod;
}

const float &SearchModeRoadblockRadius() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SearchModeRoadblockRadius;
}

};

} // namespace Gen
} // namespace Attrib

#endif
